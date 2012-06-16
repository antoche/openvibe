PROJECT(OpenViBE-kernel)

SET(PROJECT_VERSION_MAJOR x)
SET(PROJECT_VERSION_MINOR x)
SET(PROJECT_VERSION_PATCH x)
SET(PROJECT_VERSION OMK-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

# ---------------------------------
# Target macros
# Defines target operating system
# Defines target architecture
# Defines target compilator
# ---------------------------------
IF(WIN32)
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
	ADD_DEFINITIONS(-DTARGET_OS_Windows)
	ADD_DEFINITIONS(-DTARGET_ARCHITECTURE_i386)
	ADD_DEFINITIONS(-DTARGET_COMPILATOR_VisualStudio)
ENDIF(WIN32)
IF(UNIX)
	# ADD_DEFINITIONS(-fvisibility=hidden) # This flag should be present... man gcc
	ADD_DEFINITIONS(-g)
	ADD_DEFINITIONS(-fnon-call-exceptions)
	ADD_DEFINITIONS(-DTARGET_OS_Linux)
	ADD_DEFINITIONS(-DTARGET_ARCHITECTURE_i386)
	ADD_DEFINITIONS(-DTARGET_COMPILATOR_GCC)
ENDIF(UNIX)

# ---------------------------------
# Configures paths if not already defined
# Defines executable path
# Defines library output path
# Defines include output path
# ---------------------------------
IF(NOT EXECUTABLE_OUTPUT_PATH)
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
	MESSAGE(STATUS "EXECUTABLE_OUTPUT_PATH configured to default ${EXECUTABLE_OUTPUT_PATH}")
ENDIF(NOT EXECUTABLE_OUTPUT_PATH)
IF(NOT LIBRARY_OUTPUT_PATH)
	SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
	MESSAGE(STATUS "LIBRARY_OUTPUT_PATH configured to default ${LIBRARY_OUTPUT_PATH}")
ENDIF(NOT LIBRARY_OUTPUT_PATH)
SET(INCLUDE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/include)

# ------------------------------------#
# Adds configuration among link stage #
# ------------------------------------#
SET(MODULE_LINKING dynamic CACHE STRING "")
SET(OPENVIBE_LINKING dynamic CACHE STRING "")

SUBDIRS(src)
