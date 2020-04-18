# Install script for directory: C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthSplat

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
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Development/op3d_active/osgearth-3.0.0/msvc/lib/Debug/osgEarthSplatd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Development/op3d_active/osgearth-3.0.0/msvc/lib/Release/osgEarthSplat.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Development/op3d_active/osgearth-3.0.0/msvc/lib/MinSizeRel/osgEarthSplats.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Development/op3d_active/osgearth-3.0.0/msvc/lib/RelWithDebInfo/osgEarthSplatrd.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Development/op3d_active/osgearth-3.0.0/msvc/lib/Debug/osgEarthSplatd.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Development/op3d_active/osgearth-3.0.0/msvc/lib/Release/osgEarthSplat.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Development/op3d_active/osgearth-3.0.0/msvc/lib/MinSizeRel/osgEarthSplats.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Development/op3d_active/osgearth-3.0.0/msvc/lib/RelWithDebInfo/osgEarthSplatrd.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/osgEarthSplat" TYPE FILE FILES
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthSplat/Coverage"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthSplat/Export"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthSplat/GrassLayer"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthSplat/GroundCover"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthSplat/GroundCoverFeatureGenerator"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthSplat/GroundCoverFeatureSource"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthSplat/GroundCoverLayer"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthSplat/NoiseTextureFactory"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthSplat/RoadSurfaceLayer"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthSplat/SplatCoverageLegend"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthSplat/SplatCatalog"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthSplat/SplatOptions"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthSplat/SplatShaders"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthSplat/SplatLayer"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthSplat/Surface"
    "C:/Development/op3d_active/osgearth-3.0.0/src/osgEarthSplat/Zone"
    )
endif()

