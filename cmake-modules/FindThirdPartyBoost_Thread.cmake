# ---------------------------------
# Finds third party boost
# Adds library to target
# Adds include path
# ---------------------------------

INCLUDE("FindThirdPartyBoost")

IF(UNIX)
	FIND_LIBRARY(LIB_Boost_Thread NAMES "boost_thread-mt" PATHS $ENV{OpenViBE_dependencies}/lib $ENV{OpenViBE_dependencies}/lib NO_DEFAULT_PATH)
	FIND_LIBRARY(LIB_Boost_Thread NAMES "boost_thread-mt" PATHS $ENV{OpenViBE_dependencies}/lib)
	IF(LIB_Boost_Thread)
		MESSAGE(STATUS "    [  OK  ] lib ${LIB_Boost_Thread}")
		TARGET_LINK_LIBRARIES(${PROJECT_NAME}-dynamic ${LIB_Boost_Thread} )
	ELSE(LIB_Boost_Thread)
		MESSAGE(STATUS "    [FAILED] lib boost_thread-mt")
	ENDIF(LIB_Boost_Thread)
ENDIF(UNIX)

IF(WIN32)
	IF(PATH_BOOST)
		LINK_DIRECTORIES(${PATH_BOOST}/lib)
	ENDIF(PATH_BOOST)
ENDIF(WIN32)
