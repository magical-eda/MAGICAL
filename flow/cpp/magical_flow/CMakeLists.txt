cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(PROJECT_NAME "magicalFlow")
project(${PROJECT_NAME})

#The project version number
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   2   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")

# output path
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(DOCUMENT_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/docs)
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

#Check if intall prefix flag is found, if not, set a default path
if (CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX_DIR ${CMAKE_INSTALL_PREFIX})
else()
    set(CMAKE_INSTALL_PREFIX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/install)
endif()
unset(CMAKE_INSTALL_PREFIX)
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")

# Find the directory for LIMBO. If cache variable LIMBO_DIR is defined, use it as the path to LIMBO, otherwise use the system LIMBO_DIR
if (LIMBO_DIR)
    set(LIMBO_ROOT_DIR ${LIMBO_DIR})
    message(STATUS "Use LIMBO: ${LIMBO_ROOT_DIR}")
else()
    set(LIMBO_ROOT_DIR $ENV{LIMBO_DIR})
    message(STATUS "Use system LIMBO ${LIMBO_ROOT_DIR}")
endif()
unset (LIMBO_DIR CACHE)

#Gurobi
if (GUROBI_HOME)
    set(GUROBI_ROOT_DIR ${GUROBI_HOME})
    message(STATUS "Use Gurobi ${GUROBI_ROOT_DIR}")
    set(USE_GUROBI TRUE)
elseif(DEFINED ENV{GUROBI_HOME})
    set(GUROBI_ROOT_DIR $ENV{GUROBI_HOME})
    message(STATUS "Use system Gurobi ${GUROBI_ROOT_DIR}")
    set(USE_GUROBI TRUE)
else()
    message(STATUS "Disable Gurobi")
    set(USE_GUROBI FALSE)
endif()
unset(GUROBI_HOME CACHE)


if (USE_GUROBI)
    find_library(GUROBI_CPP
        NAMES gurobi_c++
        HINTS ${GUROBI_ROOT_DIR}/lib
        )
    find_library(GUROBI_C
        NAMES gurobi90
        HINTS ${GUROBI_ROOT_DIR}/lib
        )
    set(GUROBI_LIBRARIES ${GUROBI_CPP} ${GUROBI_C})
    message(STATUS "GUROBI_ROOT_DIR = ${GUROBI_ROOT_DIR}")
    message(STATUS "GUROBI_LIBRARIES = ${GUROBI_LIBRARIES}")
else()
    add_compile_definitions(LP_NOT_USE_GUROBI)
endif()


# Find the directory for LPSOLVE. If cache variable LPSOLVE_DIR is defined, use it as the path to LPSOLVE, otherwise use the system LPSOLVE_DIR
if (LPSOLVE_DIR)
    set(LPSOLVE_ROOT_DIR ${LPSOLVE_DIR})
    message(STATUS "Use LPSOLVE: ${LPSOLVE_ROOT_DIR}")
    set(USE_LPSOLVE TRUE)
elseif(DEFINED ENV{LPSOLVE_DIR})
    set(LPSOLVE_ROOT_DIR $ENV{LPSOLVE_DIR})
    message(STATUS "Use system LPSOLVE ${LPSOLVE_ROOT_DIR}")
    set(USE_LPSOLVE TRUE)
else()
    message(STATUS "Disable lpsolve")
    set(USE_LPSOLVE FALSE)
endif()
unset (LPSOLVE_DIR CACHE)

if (USE_LPSOLVE)
find_library(LPSOLVE_LIBRARIES
    NAMES liblpsolve55.so
    PATHS ${LPSOLVE_ROOT_DIR}
    )
message(STATUS "LPSOLVE_ROOT_DIR = ${LPSOLVE_ROOT_DIR}")
message(STATUS "LPSOLVE_LIBRARIES = ${LPSOLVE_LIBRARIES}")
else()
    add_compile_definitions(LP_NOT_USE_LPSOLVE)
endif()


# add a target to generate API documentation with Doxygen
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(doxy_main_page ${CMAKE_CURRENT_SOURCE_DIR}/Readme.md)
    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(doxyfile ${DOCUMENT_OUTPUT_PATH}/Doxyfile)

    #request to configure the file
    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${DOCUMENT_OUTPUT_PATH}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

    #install(DIRECTORY ${DOCUMENT_OUTPUT_PATH}/html DESTINATION ${CMAKE_INSTALL_PREFIX_DIR}/doc)
endif()
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

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

# boost
find_package(Boost 1.6 REQUIRED COMPONENTS system graph iostreams)

#Google Test
#if (GTEST_DIR)
    #set(GTEST_ROOT_DIR ${GTEST_DIR})
    #message(STATUS "Use google test: ${GTEST_ROOT_DIR}")
#else()
    #set(GTEST_ROOT_DIR $ENV{GTEST_DIR})
    #message(STATUS "Use system google test: ${GTEST_ROOT_DIR}")
#endif()
#unset(GTEST_DIR CACHE)

# Python
find_package(PythonLibs REQUIRED)

#Print out the used build type
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Using build type DEBUG")
    #ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "Using build type RELEASE")
    #ENDIF(CMAKE_BUILD_TYPE MATCHES Release)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Profile)
    message(STATUS "Using build type PROFILE")
    #ENDIF(CMAKE_BUILD_TYPE MATCHES Profile)
ELSE()
    message(STATUS "Using build type DEFAULT: using Release flag")
    set(CMAKE_BUILD_TYPE Release)
ENDIF()
#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -fopenmp")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fno-inline ")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fno-inline") 
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -frename-registers -fprofile-use -fprofile-correction") 
set(CMAKE_CXX_FLAGS_PROFILE "-Ofast -pg -Winline")
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
set(CMAKE_EXE_LINKER_FLAGS "-pthread")



# LIMBO LIB
file(GLOB LIMBO_LIB
    #limbo gdsparser
    ${LIMBO_ROOT_DIR}/lib/libgdsdb.a 
    ${LIMBO_ROOT_DIR}/lib/libgdsparser.a)


set(TO_LINK_LIBS ${Boost_LIBRARIES} ${LIMBO_LIB})



set(TO_INCLUDE_PATH 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/unittest
    #${GTEST_ROOT_DIR}/googletest/include
    ${PYBIND11_ROOT_DIR}/include
    ${PYTHON_INCLUDE_DIRS}
    ${LIMBO_ROOT_DIR}/include
)


if (USE_LPSOLVE)
    set(TO_LINK_LIBS ${TO_LINK_LIBS} ${LPSOLVE_LIBRARIES})
    set(TO_INCLUDE_PATH ${TO_INCLUDE_PATH} ${LPSOLVE_ROOT_DIR})
endif()

if (USE_GUROBI)
    set(TO_LINK_LIBS ${TO_LINK_LIBS} ${GUROBI_LIBRARIES})
    set(TO_INCLUDE_PATH ${TO_INCLUDE_PATH} ${GUROBI_ROOT_DIR}/include)
endif()

include_directories (
    ${TO_INCLUDE_PATH}
)

#file (GLOB GTEST_MAIN_LIB ${GTEST_ROOT_DIR}/lib/libgtest_main.a)
#file (GLOB GTEST_LIB ${GTEST_ROOT_DIR}/lib/libgtest.a)



link_libraries (
    ${GTEST_MAIN_LIB}
    ${PYTHON_LIBRARIES}
)

file(GLOB SOURCES src/db/*.h        src/db/*.cpp
                  src/util/*.h      src/util/*.cpp 
                  src/parser/*.h    src/parser/*.cpp
                  src/global/*.h
                  src/writer/*.h
                  src/csflow/*.h    src/csflow/*.cpp
                  src/floorplan/*.h src/floorplan/*.cpp
                  )

file(GLOB PROJECT_SOURCES src/main/*.cpp ${SOURCES})
file(GLOB PY_API_SOURCES src/api/*.cpp ${SOURCES})
#file(GLOB UNITTEST_SOURCES 
    #unittest/main/*.cpp
    #unittest/db/*.cpp
    #unittest/parser/*.cpp
    #${SOURCES})

#set_target_properties (${PROJECT_NAME} PROPERTIES LINK_FLAGS "-static")

# Add modules to pybind
pybind11_add_module(${PROJECT_NAME} ${PY_API_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIMBO_LIB} ${Boost_LIBRARIES})

#Unit Tests using GoogleTest
#enable_testing()
#add_executable(unittest ${UNITTEST_SOURCES})
#target_link_libraries(unittest ${GTEST_LIB} ${LIMBO_LIB} )
#add_test(NAME unittest COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bin/unittest ${CMAKE_CURRENT_SOURCE_DIR}/unittest)
