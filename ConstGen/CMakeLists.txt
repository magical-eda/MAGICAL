cmake_minimum_required (VERSION 2.8.11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON )
set (CMAKE_VERBOSE_MAKEFILE ON)

set(PROJECT_NAME "ConstGen")
project(${PROJECT_NAME})

set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   0   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   1   CACHE STRING "Project patch version number.")

# output path
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
#set(DOCUMENT_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/docs)
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

if (CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX_DIR ${CMAKE_INSTALL_PREFIX})
else()
    set(CMAKE_INSTALL_PREFIX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/install)
endif()
unset(CMAKE_INSTALL_PREFIX)
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")

mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

# Find the directory for LIMBO. If cache variable LIMBO_DIR is defined, use it as the path to LIMBO, otherwise use the system LIMBO_DIR
if (LIMBO_DIR)
    set(LIMBO_ROOT_DIR ${LIMBO_DIR})
    message(STATUS "Use LIMBO: ${LIMBO_ROOT_DIR}")
else()
    set(LIMBO_ROOT_DIR $ENV{LIMBO_DIR})
    message(STATUS "Use system LIMBO ${LIMBO_ROOT_DIR}")
endif()
unset (LIMBO_DIR CACHE)

#pybind11
if (PYBIND11_DIR)
    set(PYBIND11_ROOT_DIR ${PYBIND11_DIR})
    message(STATUS "Use pybind11: ${PYBIND11_ROOT_DIR}")
else()
    set(PYBIND11_ROOT_DIR $ENV{PYBIND11_DIR})
    message(STATUS "Use system pybind11: ${PYBIND11_ROOT_DIR}")
endif()
unset(PYBIND11_DIR CACHE)

add_subdirectory(${PYBIND11_ROOT_DIR} "./pybind11")

# Python
find_package(PythonLibs REQUIRED)

#Print out the used build type
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Using build type DEBUG")
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "Using build type RELEASE")
ELSEIF(CMAKE_BUILD_TYPE MATCHES Profile)
    message(STATUS "Using build type PROFILE")
ELSE()
    message(STATUS "Using build type DEFAULT: using Release flag")
    set(CMAKE_BUILD_TYPE Release)
ENDIF()
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -fopenmp -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fno-inline -fPIC")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -frename-registers -fprofile-use -fprofile-correction -fPIC") 
set(CMAKE_CXX_FLAGS_PROFILE "-Ofast -pg -Winline -fPIC")
set(CMAKE_EXE_LINKER_FLAGS "-pthread")

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${PYBIND11_ROOT_DIR}/include
    ${LIMBO_ROOT_DIR}/include
)

link_libraries (
    ${PYTHON_LIBRARIES}
)

file (GLOB SOURCES
    src/parser/*.h      src/parser/*.cpp
    src/global/*.h      src/global/*.cpp
    src/db/*.h          src/db/*.cpp
    src/sym_detect/*.h  src/sym_detect/*.cpp
    src/main/ConstGen.h src/main/DefParse.h
)

file (GLOB EXE_SOURCES src/main/main.cpp)

file(GLOB PY_API_SOURCES src/api/*.cpp ${SOURCES})

file(GLOB LIMBO_LIB 
  ${LIMBO_ROOT_DIR}/lib/libdefparseradapt.a
)

add_executable(${PROJECT_NAME} ${SOURCES} ${EXE_SOURCES})

# Add modules to pybind
pybind11_add_module(${PROJECT_NAME}Py ${PY_API_SOURCES})

target_link_libraries(${PROJECT_NAME} ${LIMBO_LIB})
target_link_libraries(${PROJECT_NAME}Py PUBLIC ${LIMBO_LIB})

install (TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX_DIR}/bin)

#add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
#set_target_properties (${PROJECT_NAME} PROPERTIES LINK_FLAGS "-static")

