
# Define Project
cmake_minimum_required(VERSION 3.2)
project(advanced-maya-playblaster VERSION 1.0.0)

# Set project version macro
add_definitions(-DPROJECT_VERSION=${PROJECT_VERSION})

# Default Maya version
if(NOT DEFINED MAYA_VERSION)
    set(MAYA_VERSION 2022 CACHE STRING "Maya version")
endif()

##############################
# Get our list of header files
set(HEADER_FILES
    src/advancedMayaPlayblast.h
)

# Get our list of source files
set(SOURCE_FILES
    src/advancedMayaPlayblast.cpp
    src/pluginMain.cpp
)

##############################



# Normally add some prefix macros, but not needed.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Get our packages
find_package(Maya REQUIRED)
find_package(Qt5 COMPONENTS Core Gui OpenGL Widgets REQUIRED)
qt5_wrap_cpp(HEADER_MOC ${HEADER_FILES})

# Compile flags
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    add_compile_options(/wd4068 /EHsc)
endif()

# Target
add_library(advancedMayaPlayblast SHARED
    ${HEADER_FILES}
    ${SOURCE_FILES}
    ${HEADER_MOC}
)

set(QT_LIBS
    Qt5::Core
    Qt5::Gui
    Qt5::OpenGL
    Qt5::Widgets
)

include_directories(${MAYA_INCLUDE_DIR})
link_directories(${MAYA_LIBRARY_DIR})

target_link_libraries(advancedMayaPlayblast PUBLIC ${MAYA_LIBRARIES} ${QT_LIBS})
MAYA_PLUGIN(advancedMayaPlayblast)

if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8" OR "${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
    add_custom_command(
        TARGET advancedMayaPlayblast POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt5::Widgets>
        $<TARGET_FILE:Qt5::Core>
        $<TARGET_FILE:Qt5::Gui>
        $<TARGET_FILE:Qt5::OpenGL>
        $<TARGET_FILE_DIR:advancedMayaPlayblast>
    )
endif()

# Set installation directory
if(NOT DEFINED INSTALL_DIR)
    set(INSTALL_DIR ${CMAKE_SOURCE_DIR}/dist)
endif()

configure_file(AdvancedMayaPlayblast.mod.in AdvancedMayaPlayblast.mod)

install(TARGETS advancedMayaPlayblast
    LIBRARY DESTINATION ${INSTALL_DIR}/AdvancedMayaPlayblast/plug-ins/${MAYA_VERSION}
    RUNTIME DESTINATION ${INSTALL_DIR}/AdvancedMayaPlayblast/plug-ins/${MAYA_VERSION})
install(FILES ${CMAKE_BINARY_DIR}/AdvancedMayaPlayblast.mod DESTINATION ${INSTALL_DIR})