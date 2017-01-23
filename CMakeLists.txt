# This is the cmake file. It describes how to build our app
# The documentation for how to build these can be found here:
# http://www.cmake.org/cmake/help/cmake-2-8-docs.html

# CMake likes us to put this here in case someone tries to read this file with an older version of cmake
cmake_minimum_required(VERSION 2.8) # http://www.cmake.org/cmake/help/cmake-2-8-docs.html#command:cmake_minimum_required

# Sets the name of the project. Additionally this sets the variables <projectName>_BINARY_DIR and <projectName>_SOURCE_DIR to the respective values.
project ("jsonDb") # http://www.cmake.org/cmake/help/cmake-2-8-docs.html#command:project

file(GLOB_RECURSE source "src/*.cpp")
file(GLOB_RECURSE header "src/*.h")

# By default we'll be a debug app
# set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_FLAGS "-std=c++11 ")

find_library(jansson jansson)

# This tells cmake that we want to create an executable called 'terrawt' and the source files are just 'main.cpp'
#SET_SOURCE_FILES_PROPERTIES( ${source} ${header} PROPERTIES LANGUAGE CXX )
add_library(jsonDb STATIC ${source} )
target_link_libraries(jsonDb jansson)

# Export header
install(DIRECTORY src/ DESTINATION include/jsonDb
        FILES_MATCHING PATTERN "*.h")
install(TARGETS jsonDb ARCHIVE DESTINATION lib)

