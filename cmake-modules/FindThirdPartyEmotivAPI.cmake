# ---------------------------------
# Finds Emotiv SDK
# Adds library to target
# Adds include path
# ---------------------------------
IF(WIN32)
# edkErrorCode.h EmoStateDLL.h
	#FIND_PATH(PATH_EmotivAPI edk.h  PATHS "C:/Program Files/Emotiv Development Kit_v1.0.0.3-PREMIUM"  "C:/Program Files/Emotiv Development Kit_v1.0.0.3-PREMIUM/doc/examples/include" "C:/Program Files (x86)/Emotiv Development Kit_v1.0.0.3-PREMIUM" "C:/Program Files (x86)/Emotiv Development Kit_v1.0.0.3-PREMIUM/doc/examples/include" $ENV{OpenViBE_dependencies})
	FIND_PATH(PATH_EmotivAPI edk.h  PATHS 
		"C:/Program Files/Emotiv Development Kit_v1.0.0.3-PREMIUM"  
		"C:/Program Files/Emotiv Development Kit_v1.0.0.3-PREMIUM/doc/examples/include" 
		"C:\Program Files\Emotiv Research Edition SDK_v1.0.0.4-PREMIUM" 
		"C:\Program Files\Emotiv Research Edition SDK_v1.0.0.4-PREMIUM/doc/examples/include"
		"C:\Program Files (x86)\Emotiv Research Edition SDK_v1.0.0.4-PREMIUM" 
		"C:\Program Files (x86)\Emotiv Research Edition SDK_v1.0.0.4-PREMIUM/doc/examples/include" $ENV{OpenViBE_dependencies})
	IF(PATH_EmotivAPI)
		MESSAGE(STATUS "  Found Emotiv API...")
		INCLUDE_DIRECTORIES(${PATH_EmotivAPI})
		FIND_LIBRARY(LIB_EmotivAPI edk PATHS "${PATH_EmotivAPI}/../lib")
		IF(LIB_EmotivAPI)
			MESSAGE(STATUS "    [  OK  ] lib ${LIB_EmotivAPI}")
			TARGET_LINK_LIBRARIES(${PROJECT_NAME}-dynamic ${LIB_EmotivAPI} )
			# To delay load EDK.dll and redistribute binary
			TARGET_LINK_LIBRARIES(${PROJECT_NAME}-dynamic Delayimp.lib )
			SET_TARGET_PROPERTIES(${PROJECT_NAME}-dynamic PROPERTIES LINK_FLAGS "/DELAYLOAD:edk.dll")
		ELSE(LIB_EmotivAPI)
			MESSAGE(STATUS "    [FAILED] lib Emotiv edk.lib")
		ENDIF(LIB_EmotivAPI)

		# Copying the DLL files at postbuild : not needed anymore cf delay load library
		# ADD_CUSTOM_COMMAND(
				# TARGET ${PROJECT_NAME}-dynamic
				# POST_BUILD
				# COMMAND ${CMAKE_COMMAND}
				# ARGS -E copy "${PATH_EmotivAPI}/../bin/edk.dll" "${PROJECT_SOURCE_DIR}/bin"
				# COMMENT "      --->   Copying dll file ${PATH_EmotivAPI}/../bin/edk.dll for the Emotiv EPOC driver."
			# VERBATIM)
		# ADD_CUSTOM_COMMAND(
				# TARGET ${PROJECT_NAME}-dynamic
				# POST_BUILD
				# COMMAND ${CMAKE_COMMAND}
				# ARGS -E copy "${PATH_EmotivAPI}/../bin/edk_utils.dll" "${PROJECT_SOURCE_DIR}/bin"
				# COMMENT "      --->   Copying dll file ${PATH_EmotivAPI}/../bin/edk_utils.dll for the Emotiv EPOC driver."
			# VERBATIM)
		
		ADD_DEFINITIONS(-DTARGET_HAS_ThirdPartyEmotivAPI)
	ELSE(PATH_EmotivAPI)
		MESSAGE(STATUS "  FAILED to find Emotiv API")
	ENDIF(PATH_EmotivAPI)
ENDIF(WIN32)
