# This is the cmake file. It describes how to build our app
# The documentation for how to build these can be found here:
# http://www.cmake.org/cmake/help/cmake-2-8-docs.html

# CMake likes us to put this here in case someone tries to read this file with an older version of cmake
cmake_minimum_required(VERSION 2.8) # http://www.cmake.org/cmake/help/cmake-2-8-docs.html#command:cmake_minimum_required

# Sets the name of the project. Additionally this sets the variables <projectName>_BINARY_DIR and <projectName>_SOURCE_DIR to the respective values.
project ("elladanJsonDb") # http://www.cmake.org/cmake/help/cmake-2-8-docs.html#command:project

file(GLOB_RECURSE source "src/*.cpp")
file(GLOB_RECURSE header "src/*.h")

# By default we'll be a debug app
# set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_FLAGS "-std=c++14 ")

set (PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_INCLUDE_DIR}")

# This tells cmake that we want to create a library called 'jsonDb'.
SET_SOURCE_FILES_PROPERTIES( ${source} ${header} PROPERTIES LANGUAGE CXX )
add_library(ElladanJsonDb STATIC ${source} )

find_library(ElladanJson ElladanJson)
find_library(ElladanHelper ElladanHelper)
target_link_libraries(ElladanJsonDb ElladanJson ElladanHelper)

# Export header
install(DIRECTORY src/ DESTINATION include/elladan/jsonDb
        FILES_MATCHING PATTERN "*.h")
install(TARGETS ElladanJsonDb ARCHIVE DESTINATION lib)


enable_testing()

add_executable(dbTest test/DbTest.cpp)
target_link_libraries(dbTest ElladanJsonDb ElladanJson ElladanHelper)
add_test(dbTest dbTest)

add_executable(SFINAETest test/SFINAETest.cpp)
target_link_libraries(SFINAETest ElladanJsonDb ElladanJson ElladanHelper)
add_test(SFINAETest SFINAETest)

add_executable(SerTest test/SerializationTest.cpp)
target_link_libraries(SerTest ElladanJsonDb ElladanJson ElladanHelper)
add_test(SerTest SerTest)
