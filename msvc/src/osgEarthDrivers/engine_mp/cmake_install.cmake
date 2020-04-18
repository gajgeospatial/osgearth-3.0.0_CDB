# Install script for directory: C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/OSGEARTH")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/osgPlugins-3.6.5" TYPE MODULE FILES "C:/Development/op3d_active/osgearth-3.0.0/msvc/lib/Debug/osgdb_osgearth_engine_mpd.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/osgPlugins-3.6.5" TYPE MODULE FILES "C:/Development/op3d_active/osgearth-3.0.0/msvc/lib/Release/osgdb_osgearth_engine_mp.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/osgPlugins-3.6.5" TYPE MODULE FILES "C:/Development/op3d_active/osgearth-3.0.0/msvc/lib/MinSizeRel/osgdb_osgearth_engine_mps.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/osgPlugins-3.6.5" TYPE MODULE FILES "C:/Development/op3d_active/osgearth-3.0.0/msvc/lib/RelWithDebInfo/osgdb_osgearth_engine_mprd.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/osgEarthDrivers/engine_mp" TYPE FILE FILES
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/Common"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/DynamicLODScaleCallback"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/HeightFieldCache"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/FileLocationCallback"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/KeyNodeFactory"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/MapFrame"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/MPGeometry"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/MPShaders"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/MPTerrainEngineNode"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/MPTerrainEngineOptions"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/SingleKeyNodeFactory"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/TerrainNode"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/TileGroup"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/TileModel"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/TileModelCompiler"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/TileNode"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/TileNodeRegistry"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/TileModelFactory"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_mp/TilePagedLOD"
    )
endif()

