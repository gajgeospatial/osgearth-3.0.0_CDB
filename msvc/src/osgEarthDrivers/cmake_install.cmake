# Install script for directory: C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthDrivers

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/basis/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/bumpmap/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/cache_filesystem/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/cache_leveldb/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/cache_rocksdb/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/colorramp/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/detail/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/earth/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/engine_rex/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/featurefilter_intersect/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/featurefilter_join/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/kml/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/mapinspector/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/monitor/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/script_engine_duktape/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/sky_gl/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/sky_silverlining/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/sky_simple/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/template/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/terrainshader/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/webp/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/vdatum_egm2008/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/vdatum_egm84/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/vdatum_egm96/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/viewpoints/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/zip/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/engine_mp/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/fastdxt/cmake_install.cmake")
  include("C:/Development/op3d_active/osgearth-3.0.0/msvc/src/osgEarthDrivers/gltf/cmake_install.cmake")

endif()

