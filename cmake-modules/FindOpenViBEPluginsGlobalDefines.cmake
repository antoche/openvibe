# ---------------------------------
# Finds OpenViBEPluginsGlobalDefines
# Adds library to target
# Adds include path
# ---------------------------------
FIND_PATH(PATH_OpenViBEPluginsGlobalDefines ovp_global_defines.h PATHS $ENV{OpenViBE_cmake_modules})
IF(PATH_OpenViBEPluginsGlobalDefines)
	MESSAGE(STATUS "  Found OpenViBE plugins global defines...")
	INCLUDE_DIRECTORIES(${PATH_OpenViBEPluginsGlobalDefines})

	ADD_DEFINITIONS(-DTARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines)
ELSE(PATH_OpenViBEPluginsGlobalDefines)
	MESSAGE(STATUS "  FAILED to find OpenViBE plugins global defines")
ENDIF(PATH_OpenViBEPluginsGlobalDefines)
