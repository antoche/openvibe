# ---------------------------------
# Finds VAmp FirstAmp library
# Adds library to target
# Adds include path
# ---------------------------------
IF(WIN32)
	FIND_PATH(PATH_USBFirstAmpAPI FirstAmp.h PATHS "C:/Program Files/FaSDK" "C:/Program Files (x86)/FaSDK" $ENV{OpenViBE_dependencies})
	IF(PATH_USBFirstAmpAPI)
		MESSAGE(STATUS "  Found FirstAmp API...")
		INCLUDE_DIRECTORIES(${PATH_USBFirstAmpAPI})
		FIND_LIBRARY(LIB_USBFirstAmpAPI FirstAmp PATHS ${PATH_USBFirstAmpAPI} )
		IF(LIB_USBFirstAmpAPI)
			MESSAGE(STATUS "    [  OK  ] lib ${LIB_USBFirstAmpAPI}")
			TARGET_LINK_LIBRARIES(${PROJECT_NAME}-dynamic ${LIB_USBFirstAmpAPI} )
		ELSE(LIB_USBFirstAmpAPI)
			MESSAGE(STATUS "    [FAILED] lib FirstAmp")
		ENDIF(LIB_USBFirstAmpAPI)

		# Copying the DLL file at postbuild
		ADD_CUSTOM_COMMAND(
				TARGET ${PROJECT_NAME}-dynamic
				POST_BUILD
				COMMAND ${CMAKE_COMMAND}
				ARGS -E copy "${PATH_USBFirstAmpAPI}/FirstAmp.dll" "${PROJECT_SOURCE_DIR}/bin"
				COMMENT "      --->   Copying dll file ${PATH_USBFirstAmpAPI}/FirstAmp.dll for the BrainProducts VAmp driver."
			VERBATIM)
		
		ADD_DEFINITIONS(-DTARGET_HAS_ThirdPartyUSBFirstAmpAPI)
	ELSE(PATH_USBFirstAmpAPI)
		MESSAGE(STATUS "  FAILED to find FirstAmp API - cmake looked in 'C:/Program Files/FaSDK' and 'C:/Program Files (x86)/FaSDK'")
	ENDIF(PATH_USBFirstAmpAPI)
ENDIF(WIN32)
