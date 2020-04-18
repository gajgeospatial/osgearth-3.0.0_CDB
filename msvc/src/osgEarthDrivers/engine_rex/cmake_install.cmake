# Install script for directory: C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex

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
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/osgPlugins-3.6.5" TYPE MODULE FILES "C:/Development/op3d_active/osgearth-3.0.0/msvc/lib/Debug/osgdb_osgearth_engine_rexd.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/osgPlugins-3.6.5" TYPE MODULE FILES "C:/Development/op3d_active/osgearth-3.0.0/msvc/lib/Release/osgdb_osgearth_engine_rex.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/osgPlugins-3.6.5" TYPE MODULE FILES "C:/Development/op3d_active/osgearth-3.0.0/msvc/lib/MinSizeRel/osgdb_osgearth_engine_rexs.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/osgPlugins-3.6.5" TYPE MODULE FILES "C:/Development/op3d_active/osgearth-3.0.0/msvc/lib/RelWithDebInfo/osgdb_osgearth_engine_rexrd.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/osgEarthDrivers/engine_rex" TYPE FILE FILES
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/Common"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/CreateTileImplementation"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/DrawState"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/DrawTileCommand"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/GeometryPool"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/Shaders"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/RexTerrainEngineNode"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/LayerDrawable"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/LoadTileData"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/MaskGenerator"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/RenderBindings"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/SurfaceNode"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/TerrainCuller"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/TerrainRenderData"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/TileDrawable"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/TileRenderModel"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/EngineContext"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/TileNode"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/TileNodeRegistry"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/Loader"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/Unloader"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers/engine_rex/SelectionInfo"
    )
endif()

