/* -*-c++-*- */
/* osgEarth - Geospatial SDK for OpenSceneGraph
 * Copyright 2020 Pelican Mapping
 * http://osgearth.org
 *
 * osgEarth is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#include <osgEarth/CDBVectorFeatureSource>
#include <osgEarth/OgrUtils>
#include <osgEarth/GeometryUtils>
#include <osgEarth/FeatureCursor>
#include <osgEarth/Filter>
#include <osgEarth/MVT>
#include <osgEarth/Registry>
#include <cdbGlobals/cdbGlobals>
#ifdef _WIN32
#include <windows.h>
#endif
#define LC "[CDBVectorFeatureSource] "

using namespace osgEarth;
#define OGR_SCOPED_LOCK GDAL_SCOPED_LOCK

//........................................................................
static __int64 _s_CDBV_FeatureID = LONG_MAX / 2;

namespace
{
	/**
	 * Determine whether a point is valid or not.  Some shapefiles can have points that are ridiculously big, which are really invalid data
	 * but shapefiles have no way of marking the data as invalid.  So instead we check for really large values that are indiciative of something being wrong.
	 */
	inline bool isPointValid(const osg::Vec3d& v, double thresh = 1e10)
	{
		return (!v.isNaN() && osg::absolute(v.x()) < thresh && osg::absolute(v.y()) < thresh && osg::absolute(v.z()) < thresh);
	}

	/**
	 * Checks to see if all points in the Geometry are valid.
	 */
	inline bool isGeometryValid(Geometry* geometry)
	{
		if (!geometry) return false;

		if (!geometry->isValid()) return false;

		for (Geometry::const_iterator i = geometry->begin(); i != geometry->end(); ++i)
		{
			if (!isPointValid(*i))
			{
				return false;
			}
		}
		return true;
	}
}

FeatureCursorCDBV::FeatureCursorCDBV(OGRDataSourceH              dsHandle,
									OGRLayerH                   layerHandle,
									const FeatureSource *		source,
									const FeatureProfile *		profile,
									const osgEarth::Query		&query,
									const FeatureFilterChain	&filters,
									ProgressCallback *			progress,
									bool						use_spatial_rect,
									CDB_Tile_ExtentP			rect_extent) :
									_dsHandle(dsHandle),
									_layerHandle(layerHandle),
									_source(source),
									_profile(profile),
									_query(query),
									_filters(filters),
									FeatureCursor(progress),
									_use_Spatial_Rect(use_spatial_rect),
									_Spatial_Rect(rect_extent),
									_resultSetHandle(0L),
									_spatialFilter(0L),
									_chunkSize(500),
									_nextHandleToQueue(0L)
{
	Set_And_Read_Data();
}


void FeatureCursorCDBV::Add_Layer_to_Cursor(OGRDataSourceH dsHandle, OGRLayerH layerHandle)
{
	_dsHandle = dsHandle;
	_layerHandle = layerHandle;
	Set_And_Read_Data();
}

void FeatureCursorCDBV::Set_And_Read_Data(void)
{
#if 0
	Read_Data_Directly();
#endif
	{
		OGR_SCOPED_LOCK;

		std::string expr;
		std::string from = OGR_FD_GetName(OGR_L_GetLayerDefn(_layerHandle));


		std::string driverName = OGR_Dr_GetName(OGR_DS_GetDriver(_dsHandle));
		// Quote the layer name if it is a shapefile, so we can handle any weird filenames like those with spaces or hyphens.
		// Or quote any layers containing spaces for PostgreSQL
		if (driverName == "ESRI Shapefile" || from.find(" ") != std::string::npos)
		{
			std::string delim = "\"";
			from = delim + from + delim;
		}

		if (_query.expression().isSet())
		{
			// build the SQL: allow the Query to include either a full SQL statement or
			// just the WHERE clause.
			expr = _query.expression().value();

			// if the expression is just a where clause, expand it into a complete SQL expression.
			std::string temp = osgEarth::toLower(expr);

			if (temp.find("select") != 0)
			{
				std::stringstream buf;
				buf << "SELECT * FROM " << from << " WHERE " << expr;
				std::string bufStr;
				bufStr = buf.str();
				expr = bufStr;
			}
		}
		else
		{
			std::stringstream buf;
			buf << "SELECT * FROM " << from;
			expr = buf.str();
		}

		//Include the order by clause if it's set
		if (_query.orderby().isSet())
		{
			std::string orderby = _query.orderby().value();

			std::string temp = osgEarth::toLower(orderby);

			if (temp.find("order by") != 0)
			{
				std::stringstream buf;
				buf << "ORDER BY " << orderby;
				std::string bufStr;
				bufStr = buf.str();
				orderby = buf.str();
			}
			expr += (" " + orderby);
		}

		// if there's a spatial extent in the query, build the spatial filter:
		if (_use_Spatial_Rect)
		{
			OGRGeometryH ring = OGR_G_CreateGeometry(wkbLinearRing);
			OGR_G_AddPoint(ring, _Spatial_Rect->West, _Spatial_Rect->South, 0);
			OGR_G_AddPoint(ring, _Spatial_Rect->West, _Spatial_Rect->North, 0);
			OGR_G_AddPoint(ring, _Spatial_Rect->East, _Spatial_Rect->North, 0);
			OGR_G_AddPoint(ring, _Spatial_Rect->East, _Spatial_Rect->South, 0);
			OGR_G_AddPoint(ring, _Spatial_Rect->West, _Spatial_Rect->South, 0);

			_spatialFilter = OGR_G_CreateGeometry(wkbPolygon);
			OGR_G_AddGeometryDirectly(_spatialFilter, ring);
		}
		else if (_query.bounds().isSet())
		{
			OGRGeometryH ring = OGR_G_CreateGeometry(wkbLinearRing);
			OGR_G_AddPoint(ring, _query.bounds()->xMin(), _query.bounds()->yMin(), 0);
			OGR_G_AddPoint(ring, _query.bounds()->xMin(), _query.bounds()->yMax(), 0);
			OGR_G_AddPoint(ring, _query.bounds()->xMax(), _query.bounds()->yMax(), 0);
			OGR_G_AddPoint(ring, _query.bounds()->xMax(), _query.bounds()->yMin(), 0);
			OGR_G_AddPoint(ring, _query.bounds()->xMin(), _query.bounds()->yMin(), 0);

			_spatialFilter = OGR_G_CreateGeometry(wkbPolygon);
			OGR_G_AddGeometryDirectly(_spatialFilter, ring);
			// note: "Directly" above means _spatialFilter takes ownership if ring handle
		}


		OE_DEBUG << LC << "SQL: " << expr << std::endl;
		_resultSetHandle = OGR_DS_ExecuteSQL(_dsHandle, expr.c_str(), _spatialFilter, 0L);

		if (_resultSetHandle)
		{
			OGR_L_ResetReading(_resultSetHandle);
		}
	}

	readChunk();

}

void FeatureCursorCDBV::Read_Data_Directly(void)
{
	FeatureList preProcessList;

	OGR_SCOPED_LOCK;

	OGR_L_ResetReading(_layerHandle);
	OGRFeatureH feat_handle;

	OGRLayer* thislayer = (OGRLayer*)_layerHandle;
	int totalCount = thislayer->GetFeatureCount();
	int fcount = -1;
	while ((feat_handle = OGR_L_GetNextFeature(_layerHandle)) != NULL)
	{
		++fcount;
		if (feat_handle)
		{
			osg::ref_ptr<Feature> f = OgrUtils::createFeature(feat_handle, _profile.get());
			if (f.valid() && !_source->isBlacklisted(f->getFID()))
			{
				if (isGeometryValid(f->getGeometry()))
				{
					_queue.push(f);

					if (_filters.size() > 0)
					{
						preProcessList.push_back(f.release());
					}
				}
				else
				{
					OE_DEBUG << LC << "Skipping feature with invalid geometry: " << f->getGeoJSON() << std::endl;
				}
			}
			OGR_F_Destroy(feat_handle);
		}
	}

	// preprocess the features using the filter list:
	if (preProcessList.size() > 0)
	{
		FilterContext cx;
		cx.setProfile(_profile.get());

		for (FeatureFilterChain::const_iterator i = _filters.begin(); i != _filters.end(); ++i)
		{
			FeatureFilter* filter = i->get();
			cx = filter->push(preProcessList, cx);
		}
	}

}

FeatureCursorCDBV::~FeatureCursorCDBV()
{
	OGR_SCOPED_LOCK;

	if (_nextHandleToQueue)
		OGR_F_Destroy(_nextHandleToQueue);

	if (_resultSetHandle != _layerHandle)
		OGR_DS_ReleaseResultSet(_dsHandle, _resultSetHandle);

	if (_spatialFilter)
		OGR_G_DestroyGeometry(_spatialFilter);

	if (_dsHandle)
		OGRReleaseDataSource(_dsHandle);
}

bool
FeatureCursorCDBV::hasMore() const
{
	return _resultSetHandle && (_queue.size() > 0 || _nextHandleToQueue != 0L);
}

Feature*
FeatureCursorCDBV::nextFeature()
{
	if (!hasMore())
		return 0L;

	if (_queue.size() == 0 && _nextHandleToQueue)
		readChunk();

	// do this in order to hold a reference to the feature we return, so the caller
	// doesn't have to. This lets us avoid requiring the caller to use a ref_ptr when 
	// simply iterating over the cursor, making the cursor move conventient to use.
	_lastFeatureReturned = _queue.front();
	_queue.pop();

	return _lastFeatureReturned.get();
}


// reads a chunk of features into a memory cache; do this for performance
// and to avoid needing the OGR Mutex every time
void
FeatureCursorCDBV::readChunk()
{
	if (!_resultSetHandle)
		return;

	FeatureList preProcessList;

	OGR_SCOPED_LOCK;

	if (_nextHandleToQueue)
	{
		osg::ref_ptr<Feature> f = OgrUtils::createFeature(_nextHandleToQueue, _profile.get());
		if (f.valid() && !_source->isBlacklisted(f->getFID()))
		{
			f->setFID(_s_CDBV_FeatureID);
			++_s_CDBV_FeatureID;
			if (isGeometryValid(f->getGeometry()))
			{
				_queue.push(f);

				if (_filters.size() > 0)
				{
					preProcessList.push_back(f.release());
				}
			}
			else
			{
				OE_DEBUG << LC << "Skipping feature with invalid geometry: " << f->getGeoJSON() << std::endl;
			}
		}
		OGR_F_Destroy(_nextHandleToQueue);
		_nextHandleToQueue = 0L;
	}

	unsigned handlesToQueue = _chunkSize - _queue.size();
	bool resultSetEndReached = false;

	for (unsigned i = 0; i < handlesToQueue; i++)
	{
		OGRFeatureH handle = OGR_L_GetNextFeature(_resultSetHandle);
		if (handle)
		{
			osg::ref_ptr<Feature> f = OgrUtils::createFeature(handle, _profile.get());
			if (f.valid() && !_source->isBlacklisted(f->getFID()))
			{
				f->setFID(_s_CDBV_FeatureID);
				++_s_CDBV_FeatureID;
				if (isGeometryValid(f->getGeometry()))
				{
					_queue.push(f);

					if (_filters.size() > 0)
					{
						preProcessList.push_back(f.release());
					}
				}
				else
				{
					OE_DEBUG << LC << "Skipping feature with invalid geometry: " << f->getGeoJSON() << std::endl;
				}
			}
			OGR_F_Destroy(handle);
		}
		else
		{
			resultSetEndReached = true;
			break;
		}
	}

	// preprocess the features using the filter list:
	if (preProcessList.size() > 0)
	{
		FilterContext cx;
		cx.setProfile(_profile.get());

		for (FeatureFilterChain::const_iterator i = _filters.begin(); i != _filters.end(); ++i)
		{
			FeatureFilter* filter = i->get();
			cx = filter->push(preProcessList, cx);
		}
	}

	// read one more for "more" detection:
	if (!resultSetEndReached)
		_nextHandleToQueue = OGR_L_GetNextFeature(_resultSetHandle);
	else
		_nextHandleToQueue = 0L;

	//OE_NOTICE << "read " << _queue.size() << " features ... " << std::endl;
}


Config
CDBVectorFeatureSource::Options::getConfig() const
{
    Config conf = FeatureSource::Options::getConfig();
	conf.set("ogr_driver",_ogrDriver);
	conf.set("geometry", _geometryConf);
	conf.set("geometry_url", _geometryUrl);
	conf.set("layer", _layer);
    conf.set("root_dir", _rootDir);
	conf.set("limits", _Limits);
    conf.set("minlod", _minLod);
    conf.set("maxlod", _maxLod);
	conf.set("edit_support", _Edit_Support);
	conf.set("verbose", _Verbose);
	return conf;
}

void
CDBVectorFeatureSource::Options::fromConfig(const Config& conf)
{
	conf.get("ogr_driver", _ogrDriver);
	conf.get("geometry", _geometryConf);
	conf.get("geometry_url", _geometryUrl);
	conf.get("layer", _layer);
	conf.get("root_dir", _rootDir);
	conf.get("limits", _Limits);
	conf.get("minlod", _minLod);
	conf.get("maxlod", _maxLod);
	conf.get("edit_support", _Edit_Support);
	conf.get("verbose", _Verbose);
}

//........................................................................

REGISTER_OSGEARTH_LAYER(cdbvectorfeatures, CDBVectorFeatureSource);

OE_LAYER_PROPERTY_IMPL(CDBVectorFeatureSource, std::string, ogrDriver, ogrDriver);
OE_LAYER_PROPERTY_IMPL(CDBVectorFeatureSource, std::string, geometryUrl, geometryUrl);
OE_LAYER_PROPERTY_IMPL(CDBVectorFeatureSource, std::string, rootDir, rootDir);
OE_LAYER_PROPERTY_IMPL(CDBVectorFeatureSource, std::string, Limits, Limits);
OE_LAYER_PROPERTY_IMPL(CDBVectorFeatureSource, int, minLod, minLod);
OE_LAYER_PROPERTY_IMPL(CDBVectorFeatureSource, int, maxLod, maxLod);
OE_LAYER_PROPERTY_IMPL(CDBVectorFeatureSource, bool, Edit_Support, Edit_Support);
OE_LAYER_PROPERTY_IMPL(CDBVectorFeatureSource, bool, Verbose, Verbose);



Status
CDBVectorFeatureSource::openImplementation()
{
    Status parent = FeatureSource::openImplementation();
    if (parent.isError())
        return parent;

	//		osgEarth::CachePolicy::NO_CACHE.apply(_dbOptions.get());
			//ToDo when working reenable  the cache disable for development 
	if(options().layer().isSet())
	{
		_LayerName = options().layer().value();
#ifdef _DEBUG
		OE_INFO << LC << "Initializing Layer " << _LayerName << std::endl;
#endif
	}

	FeatureProfile * Feature_Profile = NULL;
	const Profile * CDBFeatureProfile = NULL;

	if (options().Edit_Support().isSet())
		_CDB_Edit_Support = options().Edit_Support().value();

	if (options().Verbose().isSet())
		_Be_Verbose = options().Verbose().value();

	if (_Be_Verbose)
	{
		OE_WARN << LC << "Initializing CDBVector Layer " << _LayerName;
	}

	if (options().Limits().isSet())
	{
		std::string cdbLimits = options().Limits().value();
		double	min_lon,
			max_lon,
			min_lat,
			max_lat;

		int count = sscanf(cdbLimits.c_str(), "%lf,%lf,%lf,%lf", &min_lon, &min_lat, &max_lon, &max_lat);
		if (count == 4)
		{
			//CDB tiles always filter to geocell boundaries
			min_lon = round(min_lon);
			min_lat = round(min_lat);
			max_lat = round(max_lat);
			max_lon = round(max_lon);
			if ((max_lon > min_lon) && (max_lat > min_lat))
			{
				unsigned tiles_x = (unsigned)(max_lon - min_lon);
				unsigned tiles_y = (unsigned)(max_lat - min_lat);
				osg::ref_ptr<const SpatialReference> src_srs;
				src_srs = SpatialReference::create("EPSG:4326");
				CDBFeatureProfile = osgEarth::Profile::create(src_srs, min_lon, min_lat, max_lon, max_lat, tiles_x, tiles_y);

				//			   Below works but same as no limits
				//			   setProfile(osgEarth::Profile::create(src_srs, -180.0, -90.0, 180.0, 90.0, min_lon, min_lat, max_lon, max_lat, 90U, 45U));
			}
		}
		if (!CDBFeatureProfile)
			OE_WARN << "Invalid Limits received by CDB Driver: Not using Limits" << std::endl;

	}
	int minLod = 0;
	int maxLod = 0;
	if (options().minLod().isSet())
		minLod = options().minLod().value();
	else
		minLod = 3;

	if (options().maxLod().isSet())
	{
		maxLod = options().maxLod().value();
		if (maxLod < minLod)
			minLod = maxLod;
	}
	else
		maxLod = minLod;

	// CDB is Always a WGS84 unprojected lat/lon profile.
	if (!CDBFeatureProfile)
		CDBFeatureProfile = osgEarth::Profile::create("EPSG:4326", "", 90U, 45U);

	Feature_Profile = new FeatureProfile(CDBFeatureProfile->getExtent());
	Feature_Profile->setTilingProfile(CDBFeatureProfile);
	// Should work for now 
	Feature_Profile->setFirstLevel(minLod);
	Feature_Profile->setMaxLevel(maxLod);

	// Make sure the root directory is set
	if (!options().rootDir().isSet())
	{
		OE_WARN << "CDB root directory not set!" << std::endl;
	}
	else
	{
		_rootString = options().rootDir().value();
	}

	if (Feature_Profile)
	{
		if (options().geoInterp().isSet())
		{
			Feature_Profile->geoInterp() = options().geoInterp().get();
		}
	}

	bool errorset = false;
	std::string Errormsg = "";
	if (!osgEarth::CDBTile::CDB_Tile::Initialize_Tile_Drivers(Errormsg))
	{
		errorset = true;
	}

	if (Feature_Profile)
	{
		setFeatureProfile(Feature_Profile);
	}
	else
	{
		return Status::Error(Status::ResourceUnavailable, "CDBVectors_FeatureSource Failed to establish a valid feature profile");
	}

    return Status::NoError;
}

void
CDBVectorFeatureSource::init()
{
    FeatureSource::init();
	_dsHandle = nullptr;
	_layerHandle = nullptr;
	_ogrDriverHandle = nullptr;
	_featureCount = 0;
	_needsSync = false;
	_writable = false;
	// _schema;
	_geometryType = osgEarth::Geometry::TYPE_UNKNOWN;
	_CDB_Edit_Support = false;
	//_cacheBin;
	//_dbOptions;
	_CDBLodNum = 0;;
	_rootString = "";
	_cacheDir = "";
	_dataSet = "_S001_T001_";
	_lat_string = "";
	_lon_string = "";
	_uref_string = "";
	_rref_string = "";
	_lod_string = "";
	_LayerName = "";
	_cur_Feature_Cnt = 0;
	_Be_Verbose = false;
}


FeatureCursor*
CDBVectorFeatureSource::createFeatureCursorImplementation(const Query& query, ProgressCallback* progress)
{
	OGRDataSourceH dsHandle = 0L;
	OGRLayerH layerHandle = 0L;

	const osgEarth::TileKey key = query.tileKey().get();
	const GeoExtent key_extent = key.getExtent();
	CDB_Tile_Type tiletype = GeoPackageMap;
	CDB_Tile_Extent tileExtent(key_extent.north(), key_extent.south(), key_extent.east(), key_extent.west());
	osgEarth::CDBTile::CDB_Tile* mainTile = NULL;
	bool subtile = false;
	if (osgEarth::CDBTile::CDB_Tile::Get_Lon_Step(tileExtent.South) == 1.0)
	{
		mainTile = new osgEarth::CDBTile::CDB_Tile(_rootString, _cacheDir, tiletype, _dataSet, &tileExtent, false, false, false);
	}
	else
	{
		CDB_Tile_Extent  CDBTile_Tile_Extent = osgEarth::CDBTile::CDB_Tile::Actual_Extent_For_Tile(tileExtent);
		mainTile = new osgEarth::CDBTile::CDB_Tile(_rootString, _cacheDir, tiletype, _dataSet, &CDBTile_Tile_Extent, false, false, false);
		mainTile->Set_SpatialFilter_Extent(tileExtent);
		subtile = true;
		if (_Be_Verbose)
		{
			printf("Sourcetile: North %lf South %lf East %lf West %lf \n", CDBTile_Tile_Extent.North, CDBTile_Tile_Extent.South,
				CDBTile_Tile_Extent.East, CDBTile_Tile_Extent.West);
		}
	}

	std::string base = mainTile->FileName();
	bool have_file = mainTile->Tile_Exists();
	if (_Be_Verbose)
	{
		if (have_file)
			OE_WARN << "Loading vector tile " << base << std::endl;
		else
			OE_WARN << "No tile foe " << base << std::endl;
	}
	// open the handles safely:
	if (have_file)
	{
		OGR_SCOPED_LOCK;

		// Each cursor requires its own DS handle so that multi-threaded access will work.
		// The cursor impl will dispose of the new DS handle.
		dsHandle = OGROpenShared(base.c_str(), 0, &_ogrDriverHandle);
		if (dsHandle)
		{
			layerHandle = openLayer(dsHandle, _LayerName);
#ifdef _DEBUG
			OE_INFO << LC << "Loading Tile " << base << std::endl;
#endif
		}
	}

	FeatureCursorCDBV * fc = nullptr;
	if (dsHandle && layerHandle)
	{
		// cursor is responsible for the OGR handles.
		FeatureCursorCDBV* fc = new FeatureCursorCDBV(
			dsHandle,
			layerHandle,
			this,
			getFeatureProfile(),
			query,
			*getFilters(),
			progress,
			subtile,
			&tileExtent);

	}

	if (dsHandle)
	{
		OGR_SCOPED_LOCK;
		OGRReleaseDataSource(dsHandle);
	}
	if(mainTile)
		delete mainTile;
	return fc;
}

OGRLayerH CDBVectorFeatureSource::openLayer(OGRDataSourceH ds, const std::string& layer)
{
	OGRLayerH h = OGR_DS_GetLayerByName(ds, layer.c_str());
	if (!h)
	{
		unsigned index = osgEarth::as<unsigned>(layer, 0);
		h = OGR_DS_GetLayer(ds, index);
	}
	return h;
}


