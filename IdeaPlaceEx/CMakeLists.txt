cmake_minimum_required(VERSION 2.8)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON )
set(CMAKE_VERBOSE_MAKEFILE ON)

set(PROJECT_NAME "IdeaPlaceEx")
project(${PROJECT_NAME})

find_package (PkgConfig)
pkg_check_modules (CAIRO cairo)

#The project version number
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   2   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")

# output path
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(DOCUMENT_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/docs)
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

#Find correct boost
#set(Boost_NO_BOOST_CMAKE ON)
#set(Boost_NO_SYSTEM_PATH ON)
#set(BOOST_ROOT $ENV{BOOST_ROOT})
#set(BOOST_LIBRARYDIR $ENV{BOOST_LIB})
#set(Boost_LIBRARY_DIRS $ENV{BOOST_LIB})
#set(Boost_INCLUDE_DIR $ENV{BOOST_INC})
#set(BOOST_INCLUDEDIR $ENV{BOOST_INC})
#set(BOOST_MIN_VERSION "1.60.0")

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

# Find the directory for sparsehash. If cache variable SPARSEHASH_DIR is defined, use it as the path to SPARSEHASH, otherwise use the system SPARSEHASH_DIR
if (SPARSEHASH_DIR)
    set(SPARSEHASH_ROOT_DIR ${SPARSEHASH_DIR})
    message(STATUS "Use SPARSEHASH: ${SPARSEHASH_ROOT_DIR}")
else()
    set(SPARSEHASH_ROOT_DIR $ENV{SPARSE_HASH_DIR})
    message(STATUS "Use system SPARSEHASH ${SPARSEHASH_ROOT_DIR}")
endif()
unset (SPARSEHASH_DIR CACHE)

find_package(Boost 1.6 COMPONENTS system graph iostreams)
find_package( ZLIB REQUIRED )



# Find the directory for WNLIB. If cache variable WNLIB_DIR is defined, use it as the path to WNLIB, otherwise use the system WNLIB_DIR
if (WNLIB_DIR)
    set(WNLIB_ROOT_DIR ${WNLIB_DIR})
    message(STATUS "Use WNLIB: ${WNLIB_ROOT_DIR}")
else()
    set(WNLIB_ROOT_DIR $ENV{WNLIB_DIR})
    message(STATUS "Use system WNLIB ${WNLIB_ROOT_DIR}")
endif()
unset (WNLIB_DIR CACHE)
find_library(WNLIB_LIBRARIES
    NAMES text.a
    HINTS ${WNLIB_ROOT_DIR}/acc
    )
message(STATUS "WNLIB_ROOT_DIR = ${WNLIB_ROOT_DIR}")
message(STATUS "WNLIB_LIBRARIES = ${WNLIB_LIBRARIES}")


# Find the directory for LPSOLVE. If cache variable LPSOLVE_DIR is defined, use it as the path to LPSOLVE, otherwise use the system LPSOLVE_DIR
if (LPSOLVE_DIR)
    set(LPSOLVE_ROOT_DIR ${LPSOLVE_DIR})
    message(STATUS "Use LPSOLVE: ${LPSOLVE_ROOT_DIR}")
else()
    set(LPSOLVE_ROOT_DIR $ENV{LPSOLVE_DIR})
    message(STATUS "Use system LPSOLVE ${LPSOLVE_ROOT_DIR}")
endif()
unset (LPSOLVE_DIR CACHE)
find_library(LPSOLVE_LIBRARIES
    NAMES liblpsolve55.so
    PATHS ${LPSOLVE_ROOT_DIR}
    )
message(STATUS "LPSOLVE_ROOT_DIR = ${LPSOLVE_ROOT_DIR}")
message(STATUS "LPSOLVE_LIBRARIES = ${LPSOLVE_LIBRARIES}")

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



                #$ENV{LEMON_INC}
#link_directories(${BOOST_LIBRARYDIR})
file(GLOB SOURCES src/global/*.h    src/global/*.cpp
                  src/db/*.h        src/db/*.cpp
                  src/util/*.h      src/util/*.cpp 
                  src/parser/*.h    src/parser/*.cpp
                  src/writer/gdsii/*.h    src/writer/gdsii/*.cpp
                  src/place/*.h    src/place/*.cpp
                  src/main/IdeaPlaceEx.h src/main/IdeaPlaceEx.cpp)

file(GLOB EXE_SOURCES src/main/main.cpp)
file(GLOB PY_API_SOURCES src/api/*.cpp)

# linking libraries
file(GLOB STATIC_LIB 
    ${LIMBO_ROOT_DIR}/lib/libgdsdb.a 
    ${LIMBO_ROOT_DIR}/lib/libgdsparser.a)


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

#
##Google Test
#if (GTEST_DIR)
#    set(GTEST_ROOT_DIR ${GTEST_DIR})
#    message(STATUS "Use google test: ${GTEST_ROOT_DIR}")
#else()
#    set(GTEST_ROOT_DIR $ENV{GTEST_DIR})
#    message(STATUS "Use system google test: ${GTEST_ROOT_DIR}")
#endif()
#unset(GTEST_DIR CACHE)
#
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
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -march=native -fopenmp ")
#set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -march=native -fopenmp -fext-numeric-literals")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fno-inline ")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fno-inline") 
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -frename-registers -fprofile-use -fprofile-correction") 
set(CMAKE_CXX_FLAGS_PROFILE "-Ofast -pg -Winline")
#set(CMAKE_EXE_LINKER_FLAGS "-pthread -static-libgcc -static-libstdc++ -static")
set(CMAKE_EXE_LINKER_FLAGS "-pthread")


add_executable(${PROJECT_NAME} ${SOURCES} ${EXE_SOURCES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
    ${CAIRO_STATIC_INCLUDE_DIRS}
    ${LIMBO_ROOT_DIR}/include
    ${SPARSEHASH_ROOT_DIR}/include
    ${WNLIB_ROOT_DIR}/acc/h
    ${LPSOLVE_ROOT_DIR} 
    ${PYTHON_INCLUDE_DIRS}
    ${PYBIND11_ROOT_DIR}/include
)

target_link_libraries(${PROJECT_NAME} 
    ${STATIC_LIB} 
    ${Boost_LIBRARIES} 
    ${ZLIB_LIBRARIES} 
    ${CAIRO_STATIC_LIBRARIES}
    ${WNLIB_LIBRARIES}
    ${LPSOLVE_LIBRARIES}
    )
#
## Add modules to pybind
pybind11_add_module("IdeaPlaceExPy" ${PY_API_SOURCES} ${SOURCES})
target_link_libraries("IdeaPlaceExPy" PUBLIC ${STATIC_LIB} ${Boost_LIBRARIES} ${ZLIB_LIBRARIES} ${CAIRO_STATIC_LIBRARIES}
    ${WNLIB_LIBRARIES}
    ${LPSOLVE_LIBRARIES}
)
#
##Install
#install (TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX_DIR}/bin)
