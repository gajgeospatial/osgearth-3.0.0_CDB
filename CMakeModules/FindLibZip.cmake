# CMake module to search for libzip
#
# Once done this will define
#
#  LIBZIP_FOUND - system has the zip library
#  LIBZIP_INCLUDE_DIRS - the zip include directories
#  LIBZIP_LIBRARY - Link this to use the zip library
#
# Copyright (c) 2017, Paul Blottiere, <paul.blottiere@oslandia.com>
# Copyright (c) 2017, Larry Shaffer, <lshaffer (at) boundlessgeo (dot) com>
#   Add support for finding zipconf.h in separate location, e.g. on macOS
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
MESSAGE("Specialized FindLibZip Called")
if( NOT $ENV{LIBZIP_LIBRARY} STREQUAL "" )
  SET(LIBZIP_LIBRARY $ENV{LIBZIP_LIBRARY})
  SET(LIBZIP_LIBRARIES ${LIBZIP_LIBRARY})

  if( NOT $ENV{LIBZIP_INCLUDE_DIR} STREQUAL "" )
     SET(LIBZIP_INCLUDE_DIR $ENV{LIBZIP_INCLUDE_DIR})
  else()
     SET(LIBZIP_INCLUDE_DIR "LibzipIncludeDir")
  endif()

  if( NOT $ENV{LIBZIP_CONF_INCLUDE_DIR} STREQUAL "" )
     SET(LIBZIP_CONF_INCLUDE_DIR $ENV{LIBZIP_CONF_INCLUDE_DIR})
  else()
     SET(LIBZIP_CONF_INCLUDE_DIR "LibzipConfigIncludeDir")
  endif()

  SET(LIBZIP_INCLUDE_DIRS ${LIBZIP_INCLUDE_DIR} ${LIBZIP_CONF_INCLUDE_DIR})
  mark_as_advanced(LIBZIP_INCLUDE_DIR)
  mark_as_advanced(LIBZIP_LIBRARY)

  SET(LIBZIP_FOUND TRUE)
  MESSAGE("FindLibZip Set By Envionment")

else()

	FIND_PATH(LIBZIP_INCLUDE_DIR
	  zip.h
	  "$ENV{LIB_DIR}/include"
	  "$ENV{INCLUDE}"
	  /usr/local/include
	  /usr/include
	)

	FIND_PATH(LIBZIP_CONF_INCLUDE_DIR
	  zipconf.h
	  "$ENV{LIB_DIR}/include"
	  "$ENV{LIB_DIR}/lib/libzip/include"
	  "$ENV{LIB}/lib/libzip/include"
	  /usr/local/lib/libzip/include
	  /usr/lib/libzip/include
	  /usr/local/include
	  /usr/include
	  "$ENV{INCLUDE}"
	)

	FIND_LIBRARY(LIBZIP_LIBRARY NAMES zip PATHS "$ENV{LIB_DIR}/lib" "$ENV{LIB}" /usr/local/lib /usr/lib )

	INCLUDE(FindPackageHandleStandardArgs)
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibZip DEFAULT_MSG
									  LIBZIP_LIBRARY LIBZIP_INCLUDE_DIR LIBZIP_CONF_INCLUDE_DIR)

	SET(LIBZIP_INCLUDE_DIRS ${LIBZIP_INCLUDE_DIR} ${LIBZIP_CONF_INCLUDE_DIR})
	MARK_AS_ADVANCED(LIBZIP_LIBRARY LIBZIP_INCLUDE_DIR LIBZIP_CONF_INCLUDE_DIR LIBZIP_INCLUDE_DIRS)

	IF (LIBZIP_FOUND)
	  MESSAGE(STATUS "Found libzip: ${LIBZIP_LIBRARY}")
	ENDIF (LIBZIP_FOUND)

endif()