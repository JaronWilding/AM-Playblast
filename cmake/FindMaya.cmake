
if(NOT DEFINED MAYA_VERSION)
    set(MAYA_VERSION 2022 CACHE STRING "Maya version")
endif()

set(MAYA_COMPILE_DEFINITIONS "REQUIRE_IOSTREAM;_BOOL")
set(MAYA_INSTALL_SUFFIX "")
set(MAYA_LIB_SUFFIX "lib")
set(MAYA_INC_SUFFIX "include")

if(WIN32)
    # Windows
    set(MAYA_PLUGIN_EXT .mll)
    set(MAYA_COMPILE_DEFINITIONS "${MAYA_COMPILE_DEFINITIONS};NT_PLUGIN")

    set(AUTODESK_DEFAULT_LOCATION "C:/Program Files/Autodesk")
    set(OPENMAYA "OpenMaya.lib")
elseif(APPLE)
    # Mac
    set(MAYA_PLUGIN_EXT .bundle)
    set(MAYA_COMPILE_DEFINITIONS "${MAYA_COMPILE_DEFINITIONS};OSMac_")
    
    set(AUTODESK_DEFAULT_LOCATION "/Applications/Autodesk")
    set(OPENMAYA "libOpenMaya.dylib")

    set(MAYA_LIB_SUFFIX "Maya.app/Contents/MacOS")
    set(MAYA_INC_SUFFIX "devkit/include")
else()
    # Linux
    set(MAYA_PLUGIN_EXT .so)
    set(MAYA_COMPILE_DEFINITIONS "${MAYA_COMPILE_DEFINITIONS};LINUX")

    set(AUTODESK_DEFAULT_LOCATION "/usr/autodesk")
    set(OPENMAYA "libOpenMaya.so")

    set(MAYA_INSTALL_SUFFIX "-x64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

# Get the Autodesk and Maya location
set(AUTODESK_LOCATION ${AUTODESK_DEFAULT_LOCATION} CACHE STRING "Root Maya installation path")
set(MAYA_LOCATION ${AUTODESK_LOCATION}/maya${MAYA_VERSION}${MAYA_INSTALL_SUFFIX})

# Set the library and include paths
find_path(MAYA_LIB_DIR ${OPENMAYA} PATHS ${MAYA_LOCATION} $ENV{MAYA_LOCATION} PATH_SUFFIXES ${MAYA_LIB_SUFFIX} DOC "Maya library path")
find_path(MAYA_INC_DIR "maya/MFn.h" PATHS ${MAYA_LOCATION} $ENV{MAYA_LOCATION} PATH_SUFFIXES ${MAYA_INC_SUFFIX} DOC "Maya include path")

# Set the default OpenMaya libraries
set(_MAYA_LIBRARIES Foundation OpenMaya OpenMayaAnim OpenMayaFX OpenMayaRender OpenMayaUI)
foreach(MAYA_LIB ${_MAYA_LIBRARIES})
    find_library(MAYA_${MAYA_LIB}_LIBRARY NAMES ${MAYA_LIB} PATHS ${MAYA_LIB_DIR} NO_DEFAULT_PATH)
    set(MAYA_LIBRARIES ${MAYA_LIBRARIES} ${MAYA_${MAYA_LIB}_LIBRARY})
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Maya DEFAULT_MSG MAYA_LOCATION MAYA_INC_DIR MAYA_LIBRARIES)

function(MAYA_PLUGIN _target)
    if(WIN32)
        set_target_properties(${_target} PROPERTIES LINK_FLAGS "/export:initializePlugin /export:uninitializePlugin")
    endif()
    set_target_properties(${_target} PROPERTIES COMPILE_DEFINITIONS "${MAYA_COMPILE_DEFINITIONS}" PREFIX "" SUFFIX ${MAYA_PLUGIN_EXT})
endfunction()