if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(ASSIMP_ARCHITECTURE "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(ASSIMP_ARCHITECTURE "win32")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
	
if(WIN32)
	set(ASSIMP_ROOT_DIR "C:/Program Files/Assimp" CACHE PATH "ASSIMP root directory")

	# Find path of each library
	find_path(assimp_INCLUDE_DIRS
		NAMES
			assimp/anim.h
		HINTS
			${ASSIMP_ROOT_DIR}/include
	)

	if(MSVC12)
		set(ASSIMP_MSVC_VERSION "vc120")
	elseif(MSVC14)	
		set(ASSIMP_MSVC_VERSION "vc140")
	endif(MSVC12)
	
	if(MSVC12 OR MSVC14)
	
		find_path(ASSIMP_LIBRARY_DIR
			NAMES
				assimp-${ASSIMP_MSVC_VERSION}-mt.lib
			HINTS
				${ASSIMP_ROOT_DIR}/lib/${ASSIMP_ARCHITECTURE}
		)

		find_path(ASSIMP_BINARY_DIR
			NAMES
				assimp-${ASSIMP_MSVC_VERSION}-mt.dll
			HINTS
				${ASSIMP_ROOT_DIR}/bin/${ASSIMP_ARCHITECTURE}
		)
		
		find_library(
			assimp_LIBRARIES
			NAMES assimp-${ASSIMP_MSVC_VERSION}-mt.lib
			PATHS ${ASSIMP_LIBRARY_DIR}
		)
		
	endif()
	
else(WIN32)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(assimp REQUIRED assimp)
endif(WIN32)

if (assimp_INCLUDE_DIRS AND assimp_LIBRARIES)
	SET(assimp_FOUND TRUE)
ENDIF (assimp_INCLUDE_DIRS AND assimp_LIBRARIES)

if (assimp_FOUND)
	if (NOT assimp_FIND_QUIETLY)
	message(STATUS "Found asset importer library: ${assimp_LIBRARIES}")
	endif (NOT assimp_FIND_QUIETLY)
else (assimp_FOUND)
	if (assimp_FIND_REQUIRED)
	message(FATAL_ERROR "Could not find asset importer library")
	endif (assimp_FIND_REQUIRED)
endif (assimp_FOUND)
