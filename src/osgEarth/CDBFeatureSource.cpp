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
#include <osgEarth/CDBFeatureSource>
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
#define LC "[CDBFeatureSource] "

using namespace osgEarth;
#define OGR_SCOPED_LOCK GDAL_SCOPED_LOCK

struct CDBFeatureEntryData {
	int	CDBLod;
	std::string FullReferenceName;
};

struct CDBUnReffedFeatureEntryData {
	int CDBLod;
	std::string ModelZipName;
	std::string TextureZipName;
	std::string ArchiveFileName;
};

typedef CDBFeatureEntryData CDBFeatureEntry;
typedef CDBUnReffedFeatureEntryData CDBUnrefEntry;
typedef std::vector<CDBFeatureEntry> CDBFeatureEntryVec;
typedef std::vector<CDBUnrefEntry> CDBUnrefEntryVec;
typedef std::map<std::string, CDBFeatureEntryVec> CDBEntryMap;
typedef std::map<std::string, CDBUnrefEntryVec> CDBUnrefEntryMap;

static CDBEntryMap				_CDBInstances;
static CDBUnrefEntryMap			_CDBUnReffedInstances;

static __int64 _s_CDB_FeatureID = 0;

//........................................................................

Config
CDBFeatureSource::Options::getConfig() const
{
    Config conf = FeatureSource::Options::getConfig();
    conf.set("root_dir", _rootDir);
	conf.set("filename", _FileName);
	conf.set("devfilename", _DevFileName);
	conf.set("limits", _Limits);
	conf.set("edit_limits", _EditLimits);
#ifdef _DO_GPKG_TESTS
	conf.set("ogc_ie_test", _OGC_IE_Test);
#endif
    conf.set("min_level", _minLevel);
    conf.set("max_level", _maxLevel);
	conf.set("inflated", _inflated);
	conf.set("geotypical", _geoTypical);
	conf.set("gs_uses_gttex", _GS_uses_GTtex);
	conf.set("edit_support", _Edit_Support);
	conf.set("no_second_ref", _No_Second_Ref);
	conf.set("gt_lod0_full_stack", _GT_LOD0_FullStack);
	conf.set("gs_lod0_full_stack", _GS_LOD0_FullStack);
	conf.set("verbose", _Verbose);
	conf.set("materials", _Enable_Subord_Material);
	conf.set("abszinm", _ABS_Z_in_M);
	return conf;
}

void
CDBFeatureSource::Options::fromConfig(const Config& conf)
{

	conf.get("root_dir", _rootDir);
	conf.get("filename", _FileName);
	conf.get("devfilename", _DevFileName);
	conf.get("limits", _Limits);
	conf.get("edit_limits", _EditLimits);
#ifdef _DO_GPKG_TESTS
	conf.get("ogc_ie_test", _OGC_IE_Test);
#endif
	conf.get("min_level", _minLevel);
	conf.get("max_level", _maxLevel);
	conf.get("inflated", _inflated);
	conf.get("geotypical", _geoTypical);
	conf.get("gs_uses_gttex", _GS_uses_GTtex);
	conf.get("edit_support", _Edit_Support);
	conf.get("no_second_ref", _No_Second_Ref);
	conf.get("gt_lod0_full_stack", _GT_LOD0_FullStack);
	conf.get("gs_lod0_full_stack", _GS_LOD0_FullStack);
	conf.get("verbose", _Verbose);
	conf.get("materials", _Enable_Subord_Material);
	conf.get("abszinm", _ABS_Z_in_M);
}

//........................................................................

REGISTER_OSGEARTH_LAYER(cdbfeatures, CDBFeatureSource);

OE_LAYER_PROPERTY_IMPL(CDBFeatureSource, std::string, rootDir, rootDir);
OE_LAYER_PROPERTY_IMPL(CDBFeatureSource, std::string, FileName, FileName);
OE_LAYER_PROPERTY_IMPL(CDBFeatureSource, std::string, DevFileName, DevFileName);
OE_LAYER_PROPERTY_IMPL(CDBFeatureSource, std::string, Limits, Limits);
OE_LAYER_PROPERTY_IMPL(CDBFeatureSource, std::string, EditLimits, EditLimits);
#ifdef _DO_GPKG_TESTS
OE_LAYER_PROPERTY_IMPL(CDBFeatureSource, std::string, OGC_IE_Test, OGC_IE_Test);
#endif
OE_LAYER_PROPERTY_IMPL(CDBFeatureSource, int, MinLevel, minLevel);
OE_LAYER_PROPERTY_IMPL(CDBFeatureSource, int, MaxLevel, maxLevel);
OE_LAYER_PROPERTY_IMPL(CDBFeatureSource, bool, inflated, inflated);
OE_LAYER_PROPERTY_IMPL(CDBFeatureSource, bool, geoTypical, geoTypical);
OE_LAYER_PROPERTY_IMPL(CDBFeatureSource, bool, GS_uses_GTtex, GS_uses_GTtex);
OE_LAYER_PROPERTY_IMPL(CDBFeatureSource, bool, Edit_Support, Edit_Support);
OE_LAYER_PROPERTY_IMPL(CDBFeatureSource, bool, No_Second_Ref, No_Second_Ref);
OE_LAYER_PROPERTY_IMPL(CDBFeatureSource, bool, GT_LOD0_FullStack, GT_LOD0_FullStack);
OE_LAYER_PROPERTY_IMPL(CDBFeatureSource, bool, GS_LOD0_FullStack, GS_LOD0_FullStack);
OE_LAYER_PROPERTY_IMPL(CDBFeatureSource, bool, Verbose, Verbose);
OE_LAYER_PROPERTY_IMPL(CDBFeatureSource, bool, Enable_Subord_Material, Enable_Subord_Material);
OE_LAYER_PROPERTY_IMPL(CDBFeatureSource, bool, ABS_Z_in_M, ABS_Z_in_M);


Status
CDBFeatureSource::openImplementation()
{
    Status parent = FeatureSource::openImplementation();
    if (parent.isError())
        return parent;

    Profile* CDBFeatureProfile = 0L;
	std::string ErrorMsg;
	osgEarth::CDBTile::CDB_Tile::Initialize_Tile_Drivers(ErrorMsg);

    if (!options().minLevel().isSet() || !options().maxLevel().isSet())
    {
        return Status(Status::ConfigurationError, "CDB driver requires a min and max level");
    }

	if (options().inflated().isSet())
		_CDB_inflated = options().inflated().value();
	if (options().GS_uses_GTtex().isSet())
		_CDB_GS_uses_GTtex = options().GS_uses_GTtex().value();
	if (options().Edit_Support().isSet())
		_CDB_Edit_Support = options().Edit_Support().value();
	if (options().No_Second_Ref().isSet())
		_CDB_No_Second_Ref = options().No_Second_Ref().value();
	if (options().GT_LOD0_FullStack().isSet())
		_GT_LOD0_FullStack = options().GT_LOD0_FullStack().value();
	if (options().GS_LOD0_FullStack().isSet())
		_GS_LOD0_FullStack = options().GS_LOD0_FullStack().value();
	if (options().geoTypical().isSet())
	{
		_CDB_geoTypical = options().geoTypical().value();
		if (_CDB_geoTypical)
		{
			if (!_CDB_inflated)
			{
				_CDB_inflated = true;
			}
		}
	}
	if (options().Verbose().isSet())
	{
		bool verbose = options().Verbose().value();
		if (verbose)
			_BE_Verbose = true;
	}
	if (options().ABS_Z_in_M().isSet())
	{
		bool z_in_m = options().ABS_Z_in_M().value();
		if (z_in_m)
			_M_Contains_ABS_Z = true;
	}
	// Make sure the root directory is set
	bool CDB_Limits = true;

	CDB_Global* gbls = CDB_Global::getInstance();
	if (_GS_LOD0_FullStack)
		gbls->Set_LOD0_GS_FullStack(_GS_LOD0_FullStack);
	if (_GT_LOD0_FullStack)
		gbls->Set_LOD0_GT_FullStack(_GT_LOD0_FullStack);

	__int64 tileKey = 0;
	if (options().FileName().isSet())
	{
		_FileName = options().FileName().value();
		_UsingFileInput = true;
		bool isWFS = (_FileName.find(".xml") != std::string::npos);
		std::string tileFileName = "";
		if (isWFS)
		{

			if (gbls->Open_Vector_File(_FileName))
			{
				if (_CDB_geoTypical)
				{
					_GTGeomemtryTableName = "gpkg:GTModelGeometry_Mda.zip";
					_GTTextureTableName = "gpkg:GTModelTexture_Mda.zip";
					if (!gbls->Load_Media(_GTGeomemtryTableName, tileKey))
					{
						OE_WARN << "GTGeometry not found in GeoPackage!" << std::endl;
					}
					if (!gbls->Load_Media(_GTTextureTableName, tileKey))
					{
						OE_WARN << "GTTexture not found in GeoPackage!" << std::endl;
					}
					_CDB_inflated = false;
				}

				if (options().DevFileName().isSet())
				{
					tileFileName = options().DevFileName().value();
				}

			}
			else
			{
				OE_WARN << "Failed to open " << _FileName << std::endl;
			}
		}
		else
			tileFileName = _FileName;

		if (!tileFileName.empty())
		{
			tileKey = osgEarth::CDBTile::CDB_Tile::Get_TileKeyValue(tileFileName);
			ModelOgrTileP filetile = NULL;
			if (!gbls->HasVectorTile(tileKey))
			{
				filetile = new ModelOgrTile(tileKey, tileFileName);
				gbls->AddVectorTile(filetile);
			}
			else
			{
				filetile = gbls->GetVectorTile(tileKey);
			}

			if (!filetile->Open())
			{
				OE_WARN << "Failed to open " << tileFileName << std::endl;
			}
			else if (!isWFS)
			{
				_GTGeomemtryTableName = "gpkg:GTModelGeometry_Mda.zip";
				_GTTextureTableName = "gpkg:GTModelTexture_Mda.zip";
				if (!gbls->Load_Media(_GTGeomemtryTableName, tileKey))
				{
					OE_WARN << "GTGeometry not found in GeoPackage!" << std::endl;
				}
				if (!gbls->Load_Media(_GTTextureTableName, tileKey))
				{
					OE_WARN << "GTTexture not found in GeoPackage!" << std::endl;
				}
				_CDB_inflated = false;
			}
		}
		CDB_Limits = false;
	}

	if (!options().rootDir().isSet())
	{
		if (!_UsingFileInput)
			OE_WARN << "CDB root directory not set!" << std::endl;
	}
	else
	{
		_rootString = options().rootDir().value();
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
			unsigned tiles_x;
			unsigned tiles_y;
			if (CDB_Limits)
			{
				min_lon = round(min_lon);
				min_lat = round(min_lat);
				max_lat = round(max_lat);
				max_lon = round(max_lon);
				if ((max_lon > min_lon) && (max_lat > min_lat))
				{
					tiles_x = (unsigned)(max_lon - min_lon);
					tiles_y = (unsigned)(max_lat - min_lat);
					//			   Below works but same as no limits
					//			   setProfile(osgEarth::Profile::create(src_srs, -180.0, -90.0, 180.0, 90.0, min_lon, min_lat, max_lon, max_lat, 90U, 45U));
				}
			}
			else
			{
//				if (tileKey != -1)
				{
					min_lon = floor(min_lon);
					min_lat = floor(min_lat);
					max_lat = ceil(max_lat);
					max_lon = ceil(max_lon);
				}
				tiles_x = 1;
				tiles_y = 1;
			}
			osg::ref_ptr<const SpatialReference> src_srs;
			src_srs = SpatialReference::create("EPSG:4326");
			CDBFeatureProfile = (Profile *)osgEarth::Profile::create(src_srs, min_lon, min_lat, max_lon, max_lat, tiles_x, tiles_y);
		}
		if (!CDBFeatureProfile)
			OE_WARN << "Invalid Limits received by CDB Driver: Not using Limits" << std::endl;

	}
	if (options().EditLimits().isSet())
	{
		std::string StrLimits = options().EditLimits().value();
		double	min_lon,
			max_lon,
			min_lat,
			max_lat;

		int count = sscanf(StrLimits.c_str(), "%lf,%lf,%lf,%lf", &min_lon, &min_lat, &max_lon, &max_lat);
		if (count == 4)
		{
			_Edit_Tile_Extent.North = max_lat;
			_Edit_Tile_Extent.South = min_lat;
			_Edit_Tile_Extent.East = max_lon;
			_Edit_Tile_Extent.West = min_lon;
			_HaveEditLimits = true;
		}
	}
#ifdef _DO_GPKG_TESTS
	if (options().OGC_IE_Test().isSet())
	{
		std::string ogcTest = options().OGC_IE_Test().value();
		OGC_IE_Tracking * OGC_IE = OGC_IE_Tracking::getInstance();
		if (ogcTest == "core")
		{
			OGC_IE->Set_Test(OGC_CDB_Core);
		}
		else if (ogcTest == "vatc_1")
		{
			OGC_IE->Set_Test(VATC_Test_1);
		}
	}
#endif

    FeatureProfile * fp = new FeatureProfile(CDBFeatureProfile->getExtent());
    fp->setFirstLevel(options().minLevel().get());
    fp->setMaxLevel(options().maxLevel().get());
    fp->setTilingProfile(CDBFeatureProfile);
    if (options().geoInterp().isSet())
    {
        fp->geoInterp() = options().geoInterp().get();
    }

    setFeatureProfile(fp);

    return Status::NoError;
}

void
CDBFeatureSource::init()
{
    FeatureSource::init();
	_CDB_inflated = false;
	_CDB_geoTypical = false;
	_CDB_GS_uses_GTtex = false;
	_CDB_No_Second_Ref = false;
	_CDB_Edit_Support = false;
	_GS_LOD0_FullStack = false;
	_GT_LOD0_FullStack = false;
	_BE_Verbose = false;
	_M_Contains_ABS_Z = false;
	_UsingFileInput = false;
	_CDBLodNum = 0;
	_rootString = "";
	_FileName = "";
	_cacheDir = "";
	_dataSet = "_S001_T001_";
	_GTGeomemtryTableName = "";
	_GTTextureTableName = "";
	_cur_Feature_Cnt = 0;
	_Materials = false;
	_HaveEditLimits = false;
}


FeatureCursor*
CDBFeatureSource::createFeatureCursorImplementation(const Query& query, ProgressCallback* progress)
{
	FeatureCursor* result = 0L;
	_cur_Feature_Cnt = 0;
	// Make sure the root directory is set
	if (!options().rootDir().isSet())
	{
		if (!_UsingFileInput)
		{
			OE_WARN << "CDB root directory not set!" << std::endl;
			return result;
		}
	}
	if (_UsingFileInput)
	{
		CDB_Global::getInstance()->Check_Done();
	}
	const osgEarth::TileKey key = query.tileKey().get();
	const GeoExtent key_extent = key.getExtent();
	CDB_Tile_Type tiletype;
	if (_CDB_geoTypical)
		tiletype = GeoTypicalModel;
	else
		tiletype = GeoSpecificModel;
#ifdef _DO_GPKG_TESTS
	OGC_IE_Tracking * OGC_IE = OGC_IE_Tracking::getInstance();
	if (OGC_IE->Get_Test() != NO_IE_Test)
	{
		OGC_IE->StartTile(tiletype);
	}
#endif
	CDB_Tile_Extent tileExtent(key_extent.north(), key_extent.south(), key_extent.east(), key_extent.west());
	osgEarth::CDBTile::CDB_Tile *mainTile = NULL;
	bool subtile = false;
	if (osgEarth::CDBTile::CDB_Tile::Get_Lon_Step(tileExtent.South) == 1.0)
	{
		mainTile = new osgEarth::CDBTile::CDB_Tile(_rootString, _cacheDir, tiletype, _dataSet, &tileExtent, false, false, false, 0, _UsingFileInput);
	}
	else
	{
		CDB_Tile_Extent  CDBTile_Tile_Extent = osgEarth::CDBTile::CDB_Tile::Actual_Extent_For_Tile(tileExtent);
		mainTile = new osgEarth::CDBTile::CDB_Tile(_rootString, _cacheDir, tiletype, _dataSet, &CDBTile_Tile_Extent, false, false, false, 0, _UsingFileInput);
		if (_HaveEditLimits)
			mainTile->Set_SpatialFilter_Extent(merge_extents(_Edit_Tile_Extent, tileExtent));
		else
			mainTile->Set_SpatialFilter_Extent(tileExtent);
		subtile = true;
		if (_BE_Verbose)
		{
			OSG_WARN << "Sourcetile: North " << CDBTile_Tile_Extent.North << " South " << CDBTile_Tile_Extent.South << " East " <<
				CDBTile_Tile_Extent.East << " West " << CDBTile_Tile_Extent.West << std::endl;
		}
	}
	if (_HaveEditLimits)
		mainTile->Set_SpatialFilter_Extent(_Edit_Tile_Extent);

	_CDBLodNum = mainTile->CDB_LOD_Num();
	if (_BE_Verbose)
	{
		OSG_WARN << "CDB Feature Cursor called with CDB LOD " << _CDBLodNum << " Tile" << std::endl;
	}
	if (_UsingFileInput)
		mainTile->Set_SpatialFilter_Extent(tileExtent);

	int Files2check = mainTile->Model_Sel_Count();
	std::string base;
	int FilesChecked = 0;
	bool dataOK = false;

	FeatureList features;
	bool have_a_file = false;

	while (FilesChecked < Files2check)
	{
		base = mainTile->FileName(FilesChecked);

		// check the blacklist:
		bool blacklisted = false;
		if (Registry::instance()->isBlacklisted(base))
		{
			blacklisted = true;
			if (_BE_Verbose)
			{
				OSG_WARN << "Tile " << base << " is blacklisted" << std::endl;
			}
		}
		if (!blacklisted)
		{
			bool have_file = mainTile->Init_Model_Tile(FilesChecked);

			OE_DEBUG << query.tileKey().get().str() << "=" << base << std::endl;

			if (have_file)
			{
				if (_BE_Verbose)
				{
					if (_CDB_geoTypical)
					{
						OSG_WARN << "Feature tile loding GeoTypical Tile " << base << std::endl;
						if (subtile)
						{
							OSG_WARN << "Subtile: North " << tileExtent.North << " South " << tileExtent.South << " East " <<
								tileExtent.East << " West " << tileExtent.West << std::endl;
						}
						else if (_UsingFileInput)
						{
							OSG_WARN << "ThisTile: North " << tileExtent.North << " South " << tileExtent.South << " East " <<
								tileExtent.East << " West " << tileExtent.West << std::endl;
						}
					}
					else
					{
						OSG_WARN << "Feature tile loding GeoSpecific Tile " << base << std::endl;
						if (subtile)
						{
							OSG_WARN << "Subtile: North " << tileExtent.North << " South " << tileExtent.South << " East " <<
								tileExtent.East << " West " << tileExtent.West << std::endl;
						}
						else if (_UsingFileInput)
						{
							OSG_WARN << "ThisTile: North " << tileExtent.North << " South " << tileExtent.South << " East " <<
								tileExtent.East << " West " << tileExtent.West << std::endl;
						}
					}
				}
				bool fileOk = getFeatures(mainTile, base, features, FilesChecked);
				if (fileOk)
				{
					if (_BE_Verbose)
					{
						OSG_WARN << "File " << base << " has " << features.size() << " Features" << std::endl;
					}
					OE_INFO << LC << "Features " << features.size() << base << std::endl;
					have_a_file = true;
				}

				if (fileOk)
					dataOK = true;
				else
				{
					if (!_UsingFileInput)
						Registry::instance()->blacklist(base);
				}
			}
		}
		++FilesChecked;
	}

	if (!have_a_file)
	{
		if (!_UsingFileInput && (Files2check > 0))
			Registry::instance()->blacklist(base);
	}
	else
	{
		if (features.size() == 0)
		{
			OGRFeatureDefn * poDefn = new OGRFeatureDefn();
			OGRFeature * feat_handle = OGRFeature::CreateFeature(poDefn);
			OGRPoint poPoint;
			poPoint.setX((tileExtent.West + tileExtent.East) * 0.5);
			poPoint.setY((tileExtent.North + tileExtent.South) * 0.5);
			feat_handle->SetGeometry(&poPoint);
			osg::ref_ptr<Feature> f = OgrUtils::createFeature((OGRFeatureH)feat_handle, getFeatureProfile());
			f->setFID(_s_CDB_FeatureID);
			++_s_CDB_FeatureID;
			f->set("osge_ignore", "true");
			features.push_back(f.release());
			OGRFeature::DestroyFeature(feat_handle);
		}
	}

	delete mainTile;

#ifdef _DO_GPKG_TESTS
	if (OGC_IE->Get_Test() != NO_IE_Test)
	{
		OGC_IE->EndTile(tiletype, features.size());
	}
#endif


	result = dataOK ? new FeatureListCursor(features) : 0L;

	return result;
}

bool CDBFeatureSource::getFeatures(osgEarth::CDBTile::CDB_Tile *mainTile, const std::string& buffer, FeatureList& features, int sel)
{
	// find the right driver for the given mime type
	OGR_SCOPED_LOCK;
	// find the right driver for the given mime type
	bool have_archive = false;
	bool have_texture_zipfile = false;
#ifdef _DEBUG
	int fubar = 0;
#endif
	std::string TileNameStr;
	if (_CDB_Edit_Support)
	{
		TileNameStr = osgDB::getSimpleFileName(buffer);
		TileNameStr = osgDB::getNameLessExtension(TileNameStr);
	}

	const SpatialReference* srs = SpatialReference::create("EPSG:4326");

	std::string ModelTextureDir = "";
	std::string ModelZipFile = "";
	std::string TextureZipFile = "";
	std::string ModelZipDir = "";
	if (_CDB_inflated)
	{
		if (!_CDB_geoTypical)
		{
			if (!mainTile->Model_Texture_Directory(ModelTextureDir))
				return false;
		}
	}
	else
	{
		if (!_CDB_geoTypical)
		{
			have_archive = mainTile->Model_Geometry_Name(ModelZipFile, sel);
			if (!have_archive)
				return false;
			have_texture_zipfile = mainTile->Model_Texture_Archive(TextureZipFile, sel);
			if (_UsingFileInput && !have_texture_zipfile)
			{
				have_texture_zipfile = !_GTTextureTableName.empty();
				TextureZipFile = _GTTextureTableName;
			}
		}
		else if (_UsingFileInput)
		{
			have_archive = !_GTGeomemtryTableName.empty();
			if (!have_archive)
				return false;
			have_texture_zipfile = !_GTTextureTableName.empty();
		}
	}
	if (!_CDB_geoTypical)
		ModelZipDir = mainTile->Model_ZipDir();
	else if (_UsingFileInput)
	{
		ModelZipDir = _GTGeomemtryTableName;
		ModelZipFile = _GTGeomemtryTableName;
		TextureZipFile = _GTTextureTableName;
	}
	bool done = false;
	bool zabsindex_set = false;
	while (!done)
	{
		OGRFeature * feat_handle;
		std::string FullModelName;
		std::string ArchiveFileName;
		std::string ModelKeyName;
		bool Model_in_Archive = false;
		bool valid_model = true;
		feat_handle = mainTile->Next_Valid_Feature(sel, _CDB_inflated, ModelKeyName, FullModelName, ArchiveFileName, Model_in_Archive);
		if (feat_handle == NULL)
		{
			done = true;
			break;
		}
		if (!Model_in_Archive)
			valid_model = false;

		CDB_Model_Runtime_Class FeatureClass = mainTile->Current_Feature_Class_Data();

		double ZoffsetPos = 0.0;
		int zsetabs = FeatureClass.ahgt;
		if (!zsetabs)
		{
			if (_M_Contains_ABS_Z)
			{
				OGRGeometry *geo = feat_handle->GetGeometryRef();
				if (wkbFlatten(geo->getGeometryType()) == wkbPoint)
				{
					OGRPoint * poPoint = (OGRPoint *)geo;
					double Mpos = poPoint->getM();
					ZoffsetPos = poPoint->getZ(); //Used as altitude offset
					poPoint->setZ(Mpos + ZoffsetPos);

				}
			}
		}

		osg::ref_ptr<Feature> f = OgrUtils::createFeature((OGRFeatureH)feat_handle, getFeatureProfile());
		f->setFID(_s_CDB_FeatureID);
		++_s_CDB_FeatureID;

		f->set("osge_basename", ModelKeyName);

		if (_CDB_Edit_Support)
		{
			std::stringstream format_stream;
			format_stream << TileNameStr << "_" << std::setfill('0')
				<< std::setw(5) << abs(_cur_Feature_Cnt);

			f->set("name", ModelKeyName);
			std::string transformName = "xform_" + format_stream.str();
			f->set("transformname", transformName);
			std::string mtypevalue;
			if (_CDB_geoTypical)
				mtypevalue = "geotypical";
			else
				mtypevalue = "geospecific";
			f->set("modeltype", mtypevalue);
			f->set("tilename", buffer);
			if (!_CDB_geoTypical)
				f->set("selection", sel);
			else
				f->set("selection", mainTile->Realsel(sel));

			f->set("bsr", FeatureClass.bsr);
			f->set("bbw", FeatureClass.bbw);
			f->set("bbl", FeatureClass.bbl);
			f->set("bbh", FeatureClass.bbh);
			f->set("zoffset", ZoffsetPos);

		}
		++_cur_Feature_Cnt;
		if (!_CDB_inflated)
		{
			f->set("osge_modelzip", ModelZipFile);
		}//end else !cdb_inflated
		if (valid_model)
		{
			if (_CDB_geoTypical)
			{
				if (_CDB_No_Second_Ref)
				{
					if (f->hasAttr("inst"))
					{
						int instanceType = f->getInt("inst");
						if (instanceType == 1)
						{
							valid_model = false;
						}
					}

				}
			}
		}
		if (valid_model)
		{
			//Ok we have everthing needed to load this model at this lod
			//Set the atribution to tell osgearth to load the model
			if (have_archive)
			{
				//Normal CDB path
				f->set("osge_modelname", ArchiveFileName);
				if (have_texture_zipfile)
					f->set("osge_texturezip", TextureZipFile);
				f->set("osge_gs_uses_gt", ModelZipDir);
				std::string referencedName;
				bool instanced = false;
				int LOD = 0;
				if (find_PreInstance(ModelKeyName, referencedName, instanced, LOD))
				{
					if (LOD < _CDBLodNum)
						f->set("osge_referencedName", referencedName);
				}
			}
			else
			{
				//GeoTypical or CDB database in development path
				f->set("osge_modelname", FullModelName);
				if (!_CDB_geoTypical)
					f->set("osge_modeltexture", ModelTextureDir);
			}
			if (_Materials)
				f->set("osge_nomultidisable", "true");
#ifdef _DEBUG
			OE_DEBUG << LC << "Model File " << FullModelName << " Set to Load" << std::endl;
#endif
		}
		else
		{
			if (!_CDB_geoTypical)
			{
				//The model does not exist at this lod. It should have been loaded previously
				//Look up the exact name used when creating the model at the lower lod
				std::string referencedName;
				bool instanced = false;
				int LOD;
				if (find_PreInstance(ModelKeyName, referencedName, instanced, LOD))
				{
					f->set("osge_modelname", referencedName);
					if (_CDB_No_Second_Ref)
						valid_model = false;
				}
				else
				{
					if (instanced)
					{
						valid_model = false;
					}
					else
					{
						if (have_archive)
						{
							bool urefinstance = false;
							if (find_UnRefInstance(ModelKeyName, ModelZipFile, ArchiveFileName, TextureZipFile, urefinstance))
							{
								//Set the attribution for osgearth to load the previously unreference model
								//Normal CDB path
								valid_model = true;
								//A little paranoid verification
								if (!validate_name(ModelZipFile))
								{
									valid_model = false;
								}
								else
									f->set("osge_modelzip", ModelZipFile);

								f->set("osge_modelname", ArchiveFileName);

								if (!_CDB_GS_uses_GTtex)
								{
									have_texture_zipfile = true;
									if (!TextureZipFile.empty())
									{
										if (!validate_name(TextureZipFile))
											have_texture_zipfile = false;
										else
											f->set("osge_texturezip", TextureZipFile);
									}
									else
										have_texture_zipfile = false;
								}
								else
									f->set("osge_gs_uses_gt", ModelZipDir);
							}
							else
							{
								//Its toast and will be a red flag in the database
								OE_INFO << LC << "Model File " << FullModelName << " not found in archive" << std::endl;
							}
						}
						else
						{
							//Its toast and will be a red flag in the database
							OE_INFO << LC << " GeoTypical Model File " << FullModelName << " not found " << std::endl;
						}
					}
				}
			}
		}
		if (f.valid() && !isBlacklisted(f->getFID()))
		{
			if (valid_model && !_CDB_geoTypical)
			{
				//We need to record this instance so that this model reference can be found when referenced in 
				//higher lods. In order for osgearth to find the model we must have the exact model name that was used
				//in either a filename or archive reference
				CDBEntryMap::iterator mi = _CDBInstances.find(ModelKeyName);
				CDBFeatureEntry NewCDBEntry;
				NewCDBEntry.CDBLod = _CDBLodNum;
				if (have_archive)
					NewCDBEntry.FullReferenceName = ArchiveFileName;
				else
					NewCDBEntry.FullReferenceName = FullModelName;

				if (mi == _CDBInstances.end())
				{
					CDBFeatureEntryVec NewCDBEntryMap;
					NewCDBEntryMap.push_back(NewCDBEntry);
					_CDBInstances.insert(std::pair<std::string, CDBFeatureEntryVec>(ModelKeyName, NewCDBEntryMap));
				}
				else
				{
					CDBFeatureEntryVec CurentCDBEntryMap = _CDBInstances[ModelKeyName];
					bool can_insert = true;
					for (CDBFeatureEntryVec::iterator vi = CurentCDBEntryMap.begin(); vi != CurentCDBEntryMap.end(); ++vi)
					{
						if (vi->CDBLod == _CDBLodNum)
						{
							can_insert = false;
							break;
						}
					}
					if (can_insert)
						_CDBInstances[ModelKeyName].push_back(NewCDBEntry);
				}
			}
			//test
			if (valid_model)
			{
				features.push_back(f.release());
			}
			else
				f.release();
		}
		mainTile->DestroyCurrentFeature(sel);
	}
	if (have_archive && !_CDB_geoTypical)
	{
		//Verify all models in the archive have been referenced
		//If not store them in unreferenced
		std::string Header = mainTile->Model_HeaderName();
		osgDB::Archive::FileNameList * archiveFileList = mainTile->Model_Archive_List();

		for (osgDB::Archive::FileNameList::const_iterator f = archiveFileList->begin(); f != archiveFileList->end(); ++f)
		{
			const std::string archiveFileName = *f;
			std::string KeyName = mainTile->Model_KeyNameFromArchiveName(archiveFileName, Header);
			if (!KeyName.empty())
			{
				CDBEntryMap::iterator mi = _CDBInstances.find(KeyName);
				if (mi == _CDBInstances.end())
				{
					//The model is not in our refernced models so add it to the unreferenced list
					//so we can find it later when it is referenced.
					//This really shouldn't happen and perhaps we will make this an option to speed things 
					//up in the future but there are unfortunatly published datasets with this condition
					//Colorodo Springs is and example
					CDBUnrefEntryMap::iterator ui = _CDBUnReffedInstances.find(KeyName);
					CDBUnrefEntry NewCDBUnRefEntry;
					NewCDBUnRefEntry.CDBLod = _CDBLodNum;
					NewCDBUnRefEntry.ArchiveFileName = archiveFileName;
					NewCDBUnRefEntry.ModelZipName = ModelZipFile;
					NewCDBUnRefEntry.TextureZipName = TextureZipFile;
					if (ui == _CDBUnReffedInstances.end())
					{
						CDBUnrefEntryVec NewCDBUnRefEntryMap;
						NewCDBUnRefEntryMap.push_back(NewCDBUnRefEntry);
						_CDBUnReffedInstances.insert(std::pair<std::string, CDBUnrefEntryVec>(KeyName, NewCDBUnRefEntryMap));
					}
					else
					{
						CDBUnrefEntryVec CurentCDBUnRefEntryMap = _CDBUnReffedInstances[KeyName];
						bool can_insert = true;
						for (CDBUnrefEntryVec::iterator vi = CurentCDBUnRefEntryMap.begin(); vi != CurentCDBUnRefEntryMap.end(); ++vi)
						{
							if (vi->CDBLod == _CDBLodNum)
							{
								can_insert = false;
								break;
							}
						}
						if (can_insert)
							_CDBUnReffedInstances[KeyName].push_back(NewCDBUnRefEntry);
					}
				}
			}
		}
	}
	return true;
}

bool CDBFeatureSource::find_PreInstance(std::string &ModelKeyName, std::string &ModelReferenceName, bool &instanced, int &LOD)
{
	//The model does not exist at this lod. It should have been loaded previously
	//Look up the exact name used when creating the model at the lower lod
	CDBEntryMap::iterator mi = _CDBInstances.find(ModelKeyName);
	if (mi != _CDBInstances.end())
	{
		//Ok we found the model select the best lod. It must be lower than our current lod
		//If the model is not found here then we will simply ignore the model until we get to an lod in which
		//we find the model. If we selected to start at an lod higher than 0 there will be quite a few models
		//that fall into this catagory
		instanced = true;
		CDBFeatureEntryVec CurentCDBEntryMap = _CDBInstances[ModelKeyName];
		bool have_lod = false;
		CDBFeatureEntryVec::iterator ci;
		int mind = 1000;
		for (CDBFeatureEntryVec::iterator vi = CurentCDBEntryMap.begin(); vi != CurentCDBEntryMap.end(); ++vi)
		{
			if (vi->CDBLod <= _CDBLodNum)
			{
				int cind = _CDBLodNum - vi->CDBLod;
				if (cind < mind)
				{
					LOD = vi->CDBLod;
					have_lod = true;
					ci = vi;
				}
			}
		}
		if (have_lod)
		{
			//Set the attribution for osgearth to find the referenced model
			ModelReferenceName = ci->FullReferenceName;
#ifdef _DEBUG
			OE_DEBUG << LC << "Model File " << ModelReferenceName << " referenced" << std::endl;
#endif
			return true;
		}
		else
		{
			OE_INFO << LC << "No Instance of " << ModelKeyName << " found to reference" << std::endl;
			return false;
		}
	}
	else
		instanced = false;
	return false;
}

bool CDBFeatureSource::find_UnRefInstance(std::string &ModelKeyName, std::string &ModelZipFile, std::string &ArchiveFileName, std::string &TextureZipFile, bool &instance)
{
	//now check and see if it is an unrefernced model from a lower LOD
	CDBUnrefEntryMap::iterator ui = _CDBUnReffedInstances.find(ModelKeyName);
	if (ui != _CDBUnReffedInstances.end())
	{
		instance = true;
		//ok we found it here
		CDBUnrefEntryVec CurentCDBUnRefMap = _CDBUnReffedInstances[ModelKeyName];
		bool have_lod = false;
		CDBUnrefEntryVec::iterator ci;
		int mind = 1000;
		for (CDBUnrefEntryVec::iterator vi = CurentCDBUnRefMap.begin(); vi != CurentCDBUnRefMap.end(); ++vi)
		{
			if (vi->CDBLod <= _CDBLodNum)
			{
				int cind = _CDBLodNum - vi->CDBLod;
				if (cind < mind)
				{
					have_lod = true;
					ci = vi;
				}
			}
		}
		if (have_lod)
		{
			//Set the attribution for osgearth to load the previously unreference model
			//Normal CDB path
			ModelZipFile = ci->ModelZipName;
			ArchiveFileName = ci->ArchiveFileName;

			if (!_CDB_GS_uses_GTtex)
			{
				TextureZipFile = ci->TextureZipName;
			}
#ifdef _DEBUG
			OE_DEBUG << LC << "Previously unrefferenced Model File " << ci->ArchiveFileName << " set to load" << std::endl;
#endif
			//Ok the model is now set to load and will be added to the referenced list 
			//lets remove it from the unreferenced list
			CurentCDBUnRefMap.erase(ci);
			if (CurentCDBUnRefMap.size() == 0)
			{
				_CDBUnReffedInstances.erase(ui);
			}
			return true;
		}
		else
		{
			OE_INFO << LC << "No Instance of " << ModelKeyName << " found to reference" << std::endl;
			return false;
		}

	}
	else
		instance = false;

	return false;
}

bool CDBFeatureSource::validate_name(std::string &filename)
{
#ifdef _WIN32
	DWORD ftyp = ::GetFileAttributes(filename.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
	{
		DWORD error = ::GetLastError();
		if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
		{
			OE_DEBUG << LC << "Model File " << filename << " not found" << std::endl;
			return false;
		}
	}
	return true;
#else
	int ftyp = ::access(filename.c_str(), F_OK);
	if (ftyp == 0)
	{
		return  true;
	}
	else
	{
		return false;
	}
#endif
}

CDB_Tile_Extent CDBFeatureSource::merge_extents(CDB_Tile_Extent baseextent, CDB_Tile_Extent tileextent)
{
	CDB_Tile_Extent Extent2use;
	Extent2use = tileextent;
	return Extent2use;
}

