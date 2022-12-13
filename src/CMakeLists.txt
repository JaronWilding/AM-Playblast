set(SOURCE_FILES
    "sampleCmd.cpp"
    "sampleCmd.h"
)

find_package(Maya REQUIRED)

include_directories(${MAYA_INC_DIR})
link_directories(${MAYA_LIB_DIR})

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${MAYA_LIBRARIES})

MAYA_PLUGIN(${PROJECT_NAME})