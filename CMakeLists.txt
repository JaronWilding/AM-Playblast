cmake_minimum_required(VERSION 3.2)
project(advanced-maya-playblaster VERSION 1.0.0)

# Set default Maya version
set(MAYA_VERSION 2022 CACHE STRING "Maya version")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

add_subdirectory(src)