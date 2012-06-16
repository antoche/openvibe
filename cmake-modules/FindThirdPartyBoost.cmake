# ---------------------------------
# Finds third party boost
# Adds library to target
# Adds include path
# ---------------------------------

FIND_PATH(PATH_BOOST "include/boost/config/auto_link.hpp" PATHS $ENV{OV_DEP_BOOST} $ENV{OpenViBE_dependencies} NO_DEFAULT_PATH)
FIND_PATH(PATH_BOOST "include/boost/config/auto_link.hpp" PATHS $ENV{OV_DEP_BOOST})

IF(PATH_BOOST)
	MESSAGE(STATUS "  Found boost...")
	INCLUDE_DIRECTORIES(${PATH_BOOST}/include)

	ADD_DEFINITIONS(-DTARGET_HAS_Boost)
ELSE(PATH_BOOST)
	MESSAGE(STATUS "  FAILED to find boost...")
ENDIF(PATH_BOOST)
