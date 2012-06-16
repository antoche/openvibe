# ---------------------------------
# Finds LUA
# Adds library to target
# Adds include path
# ---------------------------------
FIND_PATH(PATH_LUA include/lua.h PATHS $ENV{OV_DEP_LUA} $ENV{OpenViBE_dependencies} NO_DEFAULT_PATH)
FIND_PATH(PATH_LUA include/lua.h PATHS $ENV{OV_DEP_LUA} $ENV{OpenViBE_dependencies})
IF(PATH_LUA)
	MESSAGE(STATUS "  Found Lua...")
	INCLUDE_DIRECTORIES(${PATH_LUA}/include)

	SET(name_liblua lua)
	IF(WIN32)
		SET(name_liblua lua51)
	ENDIF(WIN32)

	FIND_LIBRARY(LIB_LUA ${name_liblua} PATHS ${PATH_LUA}/lib NO_DEFAULT_PATH)
	FIND_LIBRARY(LIB_LUA ${name_liblua} PATHS ${PATH_LUA}/lib)
	IF(LIB_LUA)
		MESSAGE(STATUS "    [  OK  ] lib ${LIB_LUA}")
		TARGET_LINK_LIBRARIES(${PROJECT_NAME}-dynamic ${LIB_LUA})
	ELSE(LIB_LUA)
		MESSAGE(STATUS "    [FAILED] lib ${name_liblua}")
	ENDIF(LIB_LUA)

	ADD_DEFINITIONS(-DTARGET_HAS_ThirdPartyLua)
ELSE(PATH_LUA)
	MESSAGE(STATUS "  FAILED to find Lua")
ENDIF(PATH_LUA)
