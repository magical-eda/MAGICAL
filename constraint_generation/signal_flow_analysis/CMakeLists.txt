cmake_minimum_required (VERSION 2.8.11)
project (SFA)

set (CMAKE_VERBOSE_MAKEFILE ON)

set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

include_directories (
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/unittest
)

file (GLOB SOURCES
    src/parser/*.h      src/parser/*.cpp
    src/global/*.h      src/global/*.cpp
    src/db/*.h          src/db/*.cpp
    src/sym_detect/*.h  src/sym_detect/*.cpp
)

file (GLOB PROJECT_SOURCES
    src/main/main.cpp
    ${SOURCES}
)

# Print out the used build type
message (STATUS "Using build type '${CMAKE_BUILD_TYPE}'")

set (CMAKE_CXX_FLAGS "-std=c++11 -Wall ${OpenMP_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -fno-inline")
#set (CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -funroll-loops -mfpmath=sse")
set (CMAKE_CXX_FLAGS_RELEASE "-O3")
set (CMAKE_CXX_FLAGS_PROFILE "-O3 -pg")
set (CMAKE_EXE_LINKER_FLAGS "-pthread ${OpenMP_EXE_LINKER_FLAGS}")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
set_target_properties (${PROJECT_NAME} PROPERTIES LINK_FLAGS "-static")


