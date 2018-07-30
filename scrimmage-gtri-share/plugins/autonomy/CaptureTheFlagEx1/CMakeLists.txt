#--------------------------------------------------------
# Library Creation
#--------------------------------------------------------
SET (LIBRARY_NAME CaptureTheFlagEx1_plugin)
SET (LIB_MAJOR 0)
SET (LIB_MINOR 0)
SET (LIB_RELEASE 1)

file(GLOB SRCS *.cpp)

ADD_LIBRARY(${LIBRARY_NAME} SHARED 
  ${SRCS}
  )

TARGET_LINK_LIBRARIES(${LIBRARY_NAME}
  scrimmage-gtri-share-utilities
  scrimmage-gtri-share-msgs
  ${SCRIMMAGE_LIBRARIES}
  )

SET (_soversion ${LIB_MAJOR}.${LIB_MINOR}.${LIB_RELEASE})

set_target_properties(${LIBRARY_NAME} PROPERTIES 
  SOVERSION ${LIB_MAJOR} 
  VERSION ${_soversion}
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugin_libs
  )
