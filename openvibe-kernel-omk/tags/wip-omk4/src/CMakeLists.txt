FILE(GLOB_RECURSE source_files *.cpp *.h)

#ADD_LIBRARY(${PROJECT_NAME}-static STATIC ${source_files})
ADD_LIBRARY(${PROJECT_NAME}-dynamic SHARED ${source_files})

SET_TARGET_PROPERTIES(${PROJECT_NAME}-dynamic PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR}
	COMPILE_FLAGS "-DOVK_Exports -DOVK_Shared")

INCLUDE("FindOpenViBE")
INCLUDE("FindOpenViBEModuleFS")
INCLUDE("FindOpenViBEModuleSystem")
INCLUDE("FindOpenViBEModuleXML")
INCLUDE("FindThirdPartyGlade")
INCLUDE("FindThirdPartyOpenMASK4")

# ---------------------------------
# Finds standard library dl
# Adds library to target
# Adds include path
# ---------------------------------
IF(UNIX)
	FIND_LIBRARY(LIB_STANDARD_MODULE_DL dl)
	IF(LIB_STANDARD_MODULE_DL)
		MESSAGE(STATUS "  Found dl...")
		TARGET_LINK_LIBRARIES(${PROJECT_NAME}-dynamic ${LIB_STANDARD_MODULE_DL})
	ELSE(LIB_STANDARD_MODULE_PTHREAD)
		MESSAGE(STATUS "  FAILED to find dl...")
	ENDIF(LIB_STANDARD_MODULE_DL)
ENDIF(UNIX)

# ---------------------------------
# Finds standard library pthread
# Adds library to target
# Adds include path
# ---------------------------------
IF(UNIX)
	FIND_LIBRARY(LIB_STANDARD_MODULE_PTHREAD pthread)
	IF(LIB_STANDARD_MODULE_PTHREAD)
		MESSAGE(STATUS "  Found pthread...")
		TARGET_LINK_LIBRARIES(${PROJECT_NAME}-dynamic ${LIB_STANDARD_MODULE_PTHREAD})
	ELSE(LIB_STANDARD_MODULE_PTHREAD)
		MESSAGE(STATUS "  FAILED to find pthread...")
	ENDIF(LIB_STANDARD_MODULE_PTHREAD)
ENDIF(UNIX)
