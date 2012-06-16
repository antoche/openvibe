
IF(UNIX)

  FIND_PATH(EMOKIT_INCLUDE_DIR emokit/emokit.h PATHS
      /usr/local/include
      /opt/local/include
      /usr/include
    )

  FIND_LIBRARY(EMOKIT_LIB NAMES emokit PATHS /usr/local/lib /opt/local/lib /usr/lib64)

  IF(EMOKIT_LIB AND EMOKIT_INCLUDE_DIR)
    MESSAGE(STATUS "  Found Emokit...")

    INCLUDE("FindThirdPartyLibusb-1.0")
    INCLUDE("FindThirdPartyMcrypt")

    IF(LIBUSB_1_FOUND)
      IF(Mcrypt_FOUND)
        INCLUDE_DIRECTORIES(${LIBUSB_1_INCLUDE_DIRS})
        LIST(APPEND LIBEPOC_REQUIRED_LIBS ${LIBUSB_1_LIBRARIES})
        
        INCLUDE_DIRECTORIES(${EMOKIT_INCLUDE_DIR})
        MESSAGE(STATUS "    [  OK  ] lib ${EMOKIT_LIB}")    
        SET(EMOKIT_LIBS ${EMOKIT_LIB} ${Mcrypt_LIBS} ${LIBUSB_1_LIBRARIES})
        
        TARGET_LINK_LIBRARIES(${PROJECT_NAME}-dynamic ${EMOKIT_LIBS} )
	      ADD_DEFINITIONS(-DTARGET_HAS_ThirdPartyEmokit)
      ELSE(Mcrypt_FOUND)
      	MESSAGE(STATUS "  FAILED to find mcrypt")
    	ENDIF()
    ELSE()
    	MESSAGE(STATUS "  FAILED to find libusb-1.0")
    ENDIF(LIBUSB_1_FOUND)
  ELSE()
	  MESSAGE(STATUS "  FAILED to find Emokit")
  ENDIF()
ELSE()
  MESSAGE(STATUS "  FAILED to find Emokit (Please write cmake module")
ENDIF(UNIX)

