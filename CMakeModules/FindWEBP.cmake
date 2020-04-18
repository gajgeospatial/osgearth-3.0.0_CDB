
MESSAGE("Specialized FindWebP Called")
if( NOT $ENV{WEBP_LIBRARY} STREQUAL "" )
  SET(WEBP_LIBRARY $ENV{WEBP_LIBRARY})
  SET(WEBP_LIBRARIES ${WEBP_LIBRARY})

  if( NOT $ENV{WEBP_INCLUDE_DIR} STREQUAL "" )
     SET(WEBP_INCLUDE_DIR $ENV{WEBP_INCLUDE_DIR})
  else()
     SET(WEBP_INCLUDE_DIR "WebPIncludeDir")
  endif()

  MESSAGE("FindWebP WEBP_INCLUDE_DIR " ${WEBP_INCLUDE_DIR})

  if( NOT $ENV{WEBP_LIBRARY_DEBUG} STREQUAL "" )
     SET(WEBP_LIBRARY_DEBUG $ENV{WEBP_LIBRARY_DEBUG})
  else()
     SET(WEBP_LIBRARY_DEBUG ${WEBP_LIBRARY})
  endif()

  SET(WEBP_INCLUDE_DIRS ${WEBP_INCLUDE_DIR})
  mark_as_advanced(WEBP_INCLUDE_DIR)
  mark_as_advanced(WEBP_LIBRARY)

  SET(WEBP_FOUND TRUE)
  MESSAGE("FindWebP Set By Envionment")

else()

	include(SelectLibraryConfigurations)
	include(FindPackageHandleStandardArgs)

	find_path(WEBP_INCLUDE_DIR "webp/decode.h"
 			  PATH_SUFFIXES include
			  )
          
	find_library(WEBP_LIBRARY_RELEASE NAMES webp libwebp_a libwebp
				 HINTS "${WEBP_DIR}"
				 PATH_SUFFIXES "${PATH_SUFFIXES}" ${LIBRARY_PATH_SUFFIXES}
				)


	find_library(WEBP_LIBRARY_DEBUG NAMES webp_debug libwebp libwebp_a libwebp
				 HINTS "${WEBP_DIR}"
				 PATH_SUFFIXES "${PATH_SUFFIXES}" ${LIBRARY_PATH_SUFFIXES} 
				)

	select_library_configurations(WEBP)

	find_package_handle_standard_args(WEBP DEFAULT_MSG
									  WEBP_INCLUDE_DIR)

	mark_as_advanced(WEBP_INCLUDE_DIR)

	set( WEBP_FOUND "NO" )
	if( WEBP_LIBRARY AND WEBP_INCLUDE_DIR )
		set( WEBP_FOUND "YES" )
	endif()
endif()