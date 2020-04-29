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
#include "CDB"
#include <osgEarth/Registry>
#include <osgEarth/FileUtils>
#include <osgEarth/XmlUtils>
#include <osgEarth/ImageToHeightFieldConverter>
#include <osgDB/FileUtils>

#ifdef _WIN32
#include <Windows.h>
#endif


using namespace osgEarth;
using namespace osgEarth::CDB;

#undef LC
#define LC "[CDB] "

//............................................................................

Status
CDB::Driver::open(bool &UseCache, std::string &rootDir, std::string &cacheDir, std::string &dataSet, bool &Be_Verbose,
				  bool &LightMap, bool &Materials, bool &MaterialMask)
{
	_UseCache = UseCache;
	_rootDir = rootDir;
	_cacheDir = cacheDir;
	_dataSet = dataSet;
	_Be_Verbose = Be_Verbose;
	_LightMap = LightMap;
	_Materials = Materials;
	_MaterialMask = MaterialMask;
	return STATUS_OK;
}

osgEarth::ReadResult
CDB::Driver::read(const CDB_Tile_Type tiletype,
                  const TileKey& key, 
                  ProgressCallback* progress) const
{
	osg::Image *ret_Image = NULL;

	const GeoExtent key_extent = key.getExtent();
	CDB_Tile_Extent tileExtent(key_extent.north(), key_extent.south(), key_extent.east(), key_extent.west());
	osgEarth::CDBTile::CDB_Tile *mainTile = new osgEarth::CDBTile::CDB_Tile(_rootDir, _cacheDir, tiletype, _dataSet, &tileExtent, _LightMap, _Materials, _MaterialMask);
	std::string base = mainTile->FileName();
	int cdbLod = mainTile->CDB_LOD_Num();


	if (cdbLod >= 0)
	{
		if (osgEarth::CDBTile::CDB_Tile::Get_Lon_Step(tileExtent.South) == 1.0)
		{
			if (mainTile->Tile_Exists())
			{
				if (_Be_Verbose)
				{
					if (!_Materials && !_LightMap)
						OSG_WARN << "Imagery: Loading " << base << std::endl;
					else if (_Materials)
						OSG_WARN << "Imagery: Loading " << mainTile->Subordinate2_Name() << std::endl;
					else if (_LightMap)
						OSG_WARN << "Imagery: Loading " << mainTile->Subordinate_Name() << std::endl;
				}
				mainTile->Load_Tile();
				ret_Image = mainTile->Image_From_Tile();
			}
			else
			{
				if (_Be_Verbose)
					OSG_WARN << "Imagery: Blacklisting " << base << std::endl;
				Registry::instance()->blacklist(base);
			}
		}
		else
		{
			if (mainTile->Build_Earth_Tile())
			{
				if (_Be_Verbose)
				{
					if (!_Materials && !_LightMap)
						OSG_WARN << "Imagery: Loading " << base << std::endl;
					else if (_Materials)
						OSG_WARN << "Imagery: Loading " << mainTile->Subordinate2_Name() << std::endl;
					else if (_LightMap)
						OSG_WARN << "Imagery: Loading " << mainTile->Subordinate_Name() << std::endl;
				}
				OE_DEBUG "Imagery Built Earth Tile " << key.str() << "=" << base << std::endl;
				ret_Image = mainTile->Image_From_Tile();
			}
		}
	}
	else
	{
		if (mainTile->Tile_Exists())
		{
			if (_Be_Verbose)
			{
				if (!_Materials && !_LightMap)
					OSG_WARN << "Imagery: Loading " << base << std::endl;
				else if (_Materials)
					OSG_WARN << "Imagery: Loading " << mainTile->Subordinate2_Name() << std::endl;
				else if (_LightMap)
					OSG_WARN << "Imagery: Loading " << mainTile->Subordinate_Name() << std::endl;
			}
			mainTile->Load_Tile();
			ret_Image = mainTile->Image_From_Tile();
		}
		else
		{
			if (mainTile->Build_Cache_Tile(_UseCache))
			{
				if (_Be_Verbose)
				{
					if (!_Materials && !_LightMap)
						OSG_WARN << "Imagery: Loading " << base << std::endl;
					else if (_Materials)
						OSG_WARN << "Imagery: Loading " << mainTile->Subordinate2_Name() << std::endl;
					else if (_LightMap)
						OSG_WARN << "Imagery: Loading " << mainTile->Subordinate_Name() << std::endl;
				}
				ret_Image = mainTile->Image_From_Tile();
			}
		}
	}
	delete mainTile;

#ifdef _DEBUG
	if (ret_Image)
		OE_INFO "Imagery " << key.str() << "=" << base << std::endl;
	else
		OE_INFO "Missing Imagery " << key.str() << "=" << base << std::endl;
#endif

	return ret_Image;
}

GeoHeightField
CDB::Driver::readElevation(const CDB_Tile_Type tiletype,
						   const TileKey& key, 
						   ProgressCallback* progress) const
{
	osg::HeightField* ret_Field = NULL;

	const GeoExtent key_extent = key.getExtent();
	CDB_Tile_Extent tileExtent(key_extent.north(), key_extent.south(), key_extent.east(), key_extent.west());
	osgEarth::CDBTile::CDB_Tile *mainTile = new osgEarth::CDBTile::CDB_Tile(_rootDir, _cacheDir, tiletype, _dataSet, &tileExtent, _LightMap, _Materials, _MaterialMask);
	std::string base = mainTile->FileName();
	int cdbLod = mainTile->CDB_LOD_Num();

	if (cdbLod >= 0)
	{
		if (osgEarth::CDBTile::CDB_Tile::Get_Lon_Step(tileExtent.South) == 1.0)
		{
			if (mainTile->Tile_Exists())
			{
				if (_Be_Verbose)
					OSG_WARN << "Elevation: Loading " << base << std::endl;
				mainTile->Load_Tile();
				ret_Field = mainTile->HeightField_From_Tile();
			}
			else
			{
				if (_Be_Verbose)
					OSG_WARN << "Elevation: Blacklisting " << base << std::endl;
				Registry::instance()->blacklist(base);
			}
		}
		else
		{
			if (mainTile->Build_Earth_Tile())
			{
				ret_Field = mainTile->HeightField_From_Tile();
				OE_DEBUG "Elevation Built Earth Tile " << key.str() << "=" << base << std::endl;
			}
		}
	}
	else
	{
		if (mainTile->Tile_Exists())
		{
			if (_Be_Verbose)
				OSG_WARN << "Elevation: Loading " << base << std::endl;
			mainTile->Load_Tile();
			ret_Field = mainTile->HeightField_From_Tile();
		}
		else
		{
			if (mainTile->Build_Cache_Tile(_UseCache))
			{
				if (_Be_Verbose)
					OSG_WARN << "Elevation: Built Cache tile " << base << std::endl;
				ret_Field = mainTile->HeightField_From_Tile();
			}
		}
	}
	delete mainTile;

#ifdef _DEBUG
	if (ret_Field)
		OE_INFO "Elevation " << key.str() << "=" << base << std::endl;
	else
		OE_INFO "Missing Elevation " << key.str() << "=" << base << std::endl;
#endif
	return GeoHeightField(ret_Field, key.getExtent());

}

//........................................................................

Config
CDBImageLayerOptions::getConfig() const
{
    Config conf = ImageLayer::Options::getConfig();
    conf.set("root_dir", _rootDir);
	conf.set("cache_dir", _cacheDir);
	conf.set("limits", _Limits);
	conf.set("maxcdblevel", _MaxCDBLevel);
	conf.set("num_neg_lods", _NumNegLODs);
	conf.set("verbose", _Verbose);
	conf.set("disablebathyemetry", _DisableBathemetry);
	conf.set("lightmap", _Enable_Subord_Light);
	conf.set("materials", _Enable_Subord_Material);
	conf.set("materialmask", _Enable_Subord_MaterialMask);
    return conf;
}

void
CDBImageLayerOptions::fromConfig(const Config& conf)
{
	conf.get("root_dir", _rootDir);
	conf.get("cache_dir", _cacheDir);
	conf.get("limits", _Limits);
	conf.get("maxcdblevel", _MaxCDBLevel);
	conf.get("num_neg_lods", _NumNegLODs);
	conf.get("verbose", _Verbose);
	conf.get("disablebathyemetry", _DisableBathemetry);
	conf.get("lightmap", _Enable_Subord_Light);
	conf.get("materials", _Enable_Subord_Material);
	conf.get("materialmask", _Enable_Subord_MaterialMask);
}

Config
CDBImageLayerOptions::getMetadata()
{
    return Config::readJSON( OE_MULTILINE(
        { "name" : "CDB Image Tile Service",
            "properties": [
			{ "name": "root_dir",      "description" : "Location of the CDB Root Directory", "type" : "string", "default" : "" },
			{ "name": "cache_dir", "description" : "Location of (or directory to create in) the negitive LOD cache CDB data", "type" : "string", "default" : "" },
			{ "name": "limits", "description" : "String describing the limits of the CDB to display (min-lon,min-lat,max-lon,max-lat", "type" : "string", "default" : "" },
			{ "name": "maxcdblevel", "description" : "Integer containing the maxmum CDB LOD to displaly", "type" : "int", "default" : "" },
			{ "name": "num_neg_lods", "description" : "Integer containing the number of negitive CDB LODs to include in the profile", "type" : "int", "default" : "0" },
			{ "name": "verbose", "description" : "Bool if set enables verbose loging from the driver", "type" : "bool", "default" : "false" },
			{ "name": "disablebathyemetry", "description" : "Bool if set causes the driver not to include the subordinate bathemtry layer if present", "type" : "bool", "default" : "false" },
			{ "name": "lightmap", "description" : "Bool if set causes the driver to output the subordinate lightmap layer as the image layer", "type" : "bool", "default" : "false" },
			{ "name": "materials", "description" : "Bool if set causes the driver to output the subordinate material layer as the image layer", "type" : "bool", "default" : "false" },
			{ "name": "materialmask", "description" : "Bool if set (with materials)causes the driver include a material mask in the material output layer as the image layer", "type" : "bool", "default" : "false" }
			]
        }
    ) );
}

//........................................................................

Config
CDBElevationLayerOptions::getConfig() const
{
    Config conf = ElevationLayer::Options::getConfig();
	conf.set("root_dir", _rootDir);
	conf.set("cache_dir", _cacheDir);
	conf.set("limits", _Limits);
	conf.set("maxcdblevel", _MaxCDBLevel);
	conf.set("num_neg_lods", _NumNegLODs);
	conf.set("verbose", _Verbose);
	conf.set("disablebathyemetry", _DisableBathemetry);
	return conf;
}

void
CDBElevationLayerOptions::fromConfig(const Config& conf)
{
	conf.get("root_dir", _rootDir);
	conf.get("cache_dir", _cacheDir);
	conf.get("limits", _Limits);
	conf.get("maxcdblevel", _MaxCDBLevel);
	conf.get("num_neg_lods", _NumNegLODs);
	conf.get("verbose", _Verbose);
	conf.get("disablebathyemetry", _DisableBathemetry);
}

Config
CDBElevationLayerOptions::getMetadata()
{
    return Config::readJSON( OE_MULTILINE(
        { "name" : "CDB Elevation Tile Service",
            "properties": [
            { "name": "root_dir",      "description": "Location of the CDB Root Directory", "type": "string", "default": "" },
			{ "name": "cache_dir", "description" : "Location of (or directory to create in) the negitive LOD cache CDB data", "type" : "string", "default" : "" },
			{ "name": "limits", "description" : "String describing the limits of the CDB to display (min-lon,min-lat,max-lon,max-lat", "type" : "string", "default" : "" },
			{ "name": "maxcdblevel", "description" : "Integer containing the maxmum CDB LOD to displaly", "type" : "int", "default" : "" },
			{ "name": "num_neg_lods", "description" : "Integer containing the number of negitive CDB LODs to include in the profile", "type" : "int", "default" : "0" },
			{ "name": "verbose", "description" : "Bool if set enables verbose loging from the driver", "type" : "bool", "default" : "false" },
			{ "name": "disablebathyemetry", "description" : "Bool if set causes the driver not to include the subordinate bathemtry layer if present", "type" : "bool", "default" : "false" }
            ]
        }
    ) );
}

//........................................................................

REGISTER_OSGEARTH_LAYER(cdbimage, CDBImageLayer);

OE_LAYER_PROPERTY_IMPL(CDBImageLayer, std::string, rootDir, rootDir);
OE_LAYER_PROPERTY_IMPL(CDBImageLayer, std::string, cacheDir, cacheDir);
OE_LAYER_PROPERTY_IMPL(CDBImageLayer, std::string, Limits, Limits);
OE_LAYER_PROPERTY_IMPL(CDBImageLayer, int, MaxCDBLevel, MaxCDBLevel);
OE_LAYER_PROPERTY_IMPL(CDBImageLayer, int, NumNegLODs, NumNegLODs);
OE_LAYER_PROPERTY_IMPL(CDBImageLayer, bool, Verbose, Verbose);
OE_LAYER_PROPERTY_IMPL(CDBImageLayer, bool, DisableBathemetry, DisableBathemetry);
OE_LAYER_PROPERTY_IMPL(CDBImageLayer, bool, Enable_Subord_Light, Enable_Subord_Light);
OE_LAYER_PROPERTY_IMPL(CDBImageLayer, bool, Enable_Subord_Material, Enable_Subord_Material);
OE_LAYER_PROPERTY_IMPL(CDBImageLayer, bool, Enable_Subord_MaterialMask, Enable_Subord_MaterialMask);

void
CDBImageLayer::init()
{
    ImageLayer::init();
	_UseCache = false;
	_rootDir = "";
	_cacheDir = "";
	_dataSet = "_S001_T001_";
	_tileSize = 1024;
	_Be_Verbose = false;
	_LightMap = false;
	_Materials = false;
	_MaterialMask = false;
}

Status
CDBImageLayer::openImplementation()
{
    Status parent = ImageLayer::openImplementation();
    if (parent.isError())
        return parent;

	bool errorset = false;
	std::string Errormsg = "";
	//The CDB Root directory is required.
	if (!options().rootDir().isSet())
	{
		OE_WARN << "CDB root directory not set!" << std::endl;
		Errormsg = "CDB root directory not set";
		errorset = true;
	}
	else
	{
		_rootDir = options().rootDir().value();
	}

	//Set drivers for the image/elevation layer.
	if (!osgEarth::CDBTile::CDB_Tile::Initialize_Tile_Drivers(Errormsg))
	{
		errorset = true;
	}

	if (options().DisableBathemetry().isSet())
	{
		bool disable = options().DisableBathemetry().value();
		if (disable)
			osgEarth::CDBTile::CDB_Tile::Disable_Bathyemtry(true);
	}

	if (options().Verbose().isSet())
	{
		bool verbose = options().Verbose().value();
		if (verbose)
			_Be_Verbose = true;
	}
	//Get the chache directory if it is set and turn on the cacheing option if it is present
	if (options().cacheDir().isSet())
	{
		_cacheDir = options().cacheDir().value();
		_UseCache = true;
	}

	if (options().Enable_Subord_Light().isSet())
	{
		_LightMap = options().Enable_Subord_Light().value();
	}

	if (options().Enable_Subord_Material().isSet())
	{
		_Materials = options().Enable_Subord_Material().value();
	}

	if (options().Enable_Subord_MaterialMask().isSet())
	{
		_MaterialMask = options().Enable_Subord_MaterialMask().value();
	}

	//verify tilesize
	if (options().tileSize().isSet())
		_tileSize = options().tileSize().value();

	bool profile_set = false;
	int maxcdbdatalevel = 14;
	//Check if there are limits on the maximum Lod to use
	if (options().MaxCDBLevel().isSet())
	{
		maxcdbdatalevel = options().MaxCDBLevel().value();
	}

	//Check to see if we have been told how many negitive lods to use
	int Number_of_Negitive_LODs_to_Use = 0;
	if (options().NumNegLODs().isSet())
	{
		Number_of_Negitive_LODs_to_Use = options().NumNegLODs().value();
	}

	//Check to see if we are loading only a limited area of the earth
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
			//Make sure the profile includes hole tiles above and below 50 deg
			int lonstep = osgEarth::CDBTile::CDB_Tile::Get_Lon_Step(min_lat);
			lonstep = osgEarth::CDBTile::CDB_Tile::Get_Lon_Step(max_lat) > lonstep ? osgEarth::CDBTile::CDB_Tile::Get_Lon_Step(max_lat) : lonstep;
			if (lonstep > 1)
			{
				int delta = (int)min_lon % lonstep;
				if (delta)
				{
					min_lon = (double)(((int)min_lon - lonstep) * lonstep);
				}
				delta = (int)max_lon % lonstep;
				if (delta)
				{
					max_lon = (double)(((int)max_lon + lonstep) * lonstep);
				}
			}
			//Expand the limits if necessary to meet the criteria for the number of negitive lods specified
			int subfact = 1;
			if(Number_of_Negitive_LODs_to_Use > 0)
			   subfact = 2 << Number_of_Negitive_LODs_to_Use;  //2 starts with lod 0 this means howerver a minumum of 4 geocells will be requested even if only one
			if ((max_lon > min_lon) && (max_lat > min_lat))	   //is specified in the limits section of the earth file.
			{
				unsigned tiles_x = (unsigned)(max_lon - min_lon);
				int modx = tiles_x % subfact;
				if (modx != 0)
				{
					tiles_x = ((tiles_x + subfact) / subfact) * subfact;
					max_lon = min_lon + (double)tiles_x;
				}
				tiles_x /= subfact;

				unsigned tiles_y = (unsigned)(max_lat - min_lat);
				int mody = tiles_y % subfact;
				if (mody != 0)
				{
					tiles_y = ((tiles_y + subfact) / subfact) * subfact;
					max_lat = min_lat + (double)tiles_y;
				}
				tiles_y /= subfact;

				//Create the Profile with the calculated limitations
				osg::ref_ptr<const SpatialReference> src_srs;
				src_srs = SpatialReference::create("EPSG:4326");
				GeoExtent extents = GeoExtent(src_srs, min_lon, min_lat, max_lon, max_lat);
				DataExtentList& Layerextents = (DataExtentList)getDataExtents();
				Layerextents.push_back(DataExtent(extents, 0, maxcdbdatalevel + Number_of_Negitive_LODs_to_Use + 1)); //plus number of sublevels
				setProfile(osgEarth::Profile::create(src_srs, min_lon, min_lat, max_lon, max_lat, tiles_x, tiles_y));

				OE_INFO "CDB Profile Min Lon " << min_lon << " Min Lat " << min_lat << " Max Lon " << max_lon << " Max Lat " << max_lat << "Tiles " << tiles_x << " " << tiles_y << std::endl;
				OE_INFO "  Number of negitive lods " << Number_of_Negitive_LODs_to_Use << " Subfact " << subfact << std::endl;
				profile_set = true;
			}
		}
		if (!profile_set)
			OE_WARN << "Invalid Limits received by CDB Driver: Not using Limits" << std::endl;

	}

	// Always a WGS84 unprojected lat/lon profile.
	if (!profile_set)
	{
		//Use a default world profile
		Number_of_Negitive_LODs_to_Use = 5;
		osg::ref_ptr<const SpatialReference> src_srs;
		src_srs = SpatialReference::create("EPSG:4326");
		GeoExtent extents = GeoExtent(SpatialReference::create("EPSG:4326"), -180.0, -90.0, 180.0, 90.0);
		DataExtentList& Layerextents = (DataExtentList)getDataExtents();
		Layerextents.push_back(DataExtent(extents, 0, maxcdbdatalevel + Number_of_Negitive_LODs_to_Use));
		//	   setProfile(osgEarth::Profile::create(src_srs, -180.0, -102.0, 204.0, 90.0, 6U, 3U));
		setProfile(osgEarth::Profile::create(src_srs, -180.0, -102.0, 204.0, 90.0, 12U, 6U));
		if (!_UseCache)
		{
			if (!errorset)
			{
				//Look for the Default Cache Dir
				std::stringstream buf;
				buf << _rootDir
					<< "/osgEarth"
					<< "/CDB_Cache";
				_cacheDir = buf.str();
#ifdef _WIN32
				DWORD ftyp = ::GetFileAttributes(_cacheDir.c_str());
				if (ftyp != INVALID_FILE_ATTRIBUTES)
				{
					_UseCache = true;
				}
#else
				int ftyp = ::access(_cacheDir.c_str(), F_OK);
				if (ftyp == 0)
				{
					_UseCache = true;
				}
#endif
			}
		}
	}

	if (errorset)
	{
		Status Rstatus(Errormsg);
		return Rstatus;
	}
	else
	{
		_driver.open(_UseCache, _rootDir, _cacheDir, _dataSet, _Be_Verbose, _LightMap, _Materials, _MaterialMask);
		return Status::NoError;
	}
}

GeoImage
CDBImageLayer::createImageImplementation(const TileKey& key, ProgressCallback* progress) const
{
	ReadResult r = _driver.read(Imagery, key, progress);

    if (r.succeeded())
        return GeoImage(r.releaseImage(), key.getExtent());
    else
        return GeoImage(Status(r.errorDetail()));
}

GeoHeightField
CDBImageLayer::createHeightFieldImplementation(const TileKey& key, ProgressCallback* progress) const
{
	GeoHeightField r = _driver.readElevation(Elevation, key, progress);
	if (r.getHeightField())
		return r;
	else
		return GeoHeightField::INVALID;
}
//........................................................................

REGISTER_OSGEARTH_LAYER(cdbelevation, CDBElevationLayer);

OE_LAYER_PROPERTY_IMPL(CDBElevationLayer, std::string, rootDir, rootDir);
OE_LAYER_PROPERTY_IMPL(CDBElevationLayer, std::string, cacheDir, cacheDir);
OE_LAYER_PROPERTY_IMPL(CDBElevationLayer, std::string, Limits, Limits);
OE_LAYER_PROPERTY_IMPL(CDBElevationLayer, int, MaxCDBLevel, MaxCDBLevel);
OE_LAYER_PROPERTY_IMPL(CDBElevationLayer, int, NumNegLODs, NumNegLODs);
OE_LAYER_PROPERTY_IMPL(CDBElevationLayer, bool, Verbose, Verbose);
OE_LAYER_PROPERTY_IMPL(CDBElevationLayer, bool, DisableBathemetry, DisableBathemetry);

void
CDBElevationLayer::init()
{
    ElevationLayer::init();
}

Status
CDBElevationLayer::openImplementation()
{
    Status parent = ElevationLayer::openImplementation();
    if (parent.isError())
        return parent;

    // Create an image layer under the hood. TMS fetch is the same for image and
    // elevation; we just convert the resulting image to a heightfield
    _imageLayer = new CDBImageLayer(options());

    // Initialize and open the image layer
    _imageLayer->setReadOptions(getReadOptions());
    Status status = _imageLayer->open();

    if (status.isError())
        return status;

    setProfile(_imageLayer->getProfile());            

    return Status::NoError;
}

GeoHeightField
CDBElevationLayer::createHeightFieldImplementation(const TileKey& key, ProgressCallback* progress) const
{
	return _imageLayer->createHeightFieldImplementation(key, progress);
}
