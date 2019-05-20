cmake_minimum_required(VERSION 2.8)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON )
set(CMAKE_VERBOSE_MAKEFILE ON)

#set(CMAKE_CXX_STANDARD 11)
set(PROJECT_NAME "well_generation")
project(${PROJECT_NAME})



#The project version number
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   0   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")

# output path
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(DOCUMENT_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/docs)
set(SOURCE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_ROOT_DIR ${SOURCE_ROOT_DIR})

# Find the directory for LIMBO. If cache variable LIMBO_DIR is defined, use it as the path to LIMBO, otherwise use the system LIMBO_DIR
if (LIMBO_DIR)
    set(LIMBO_ROOT_DIR ${LIMBO_DIR})
    message(STATUS "Use LIMBO: ${LIMBO_ROOT_DIR}")
else()
    set(LIMBO_ROOT_DIR $ENV{LIMBO_DIR})
    message(STATUS "Use system LIMBO ${LIMBO_ROOT_DIR}")
endif()
unset (LIMBO_DIR CACHE)
find_library(LIMBO_GDSDB
    NAMES gdsdb 
    HINTS ${LIMBO_ROOT_DIR}/lib
    )
find_library(LIMBO_GDSPARSER
    NAMES gdsparser 
    HINTS ${LIMBO_ROOT_DIR}/lib
    )
find_library(LIMBO_PROGRAMOPTIONS
    NAMES programoptions 
    HINTS ${LIMBO_ROOT_DIR}/lib
    )
find_library(LIMBO_GEOBOOSTPOLYGONAPI
	NAMES GeoBoostPolygonApi 
    HINTS ${LIMBO_ROOT_DIR}/lib
    )
set(LIMBO_LIBRARIES ${LIMBO_GDSDB} ${LIMBO_GDSPARSER} ${LIMBO_PROGRAMOPTIONS} ${LIMBO_GEOBOOSTPOLYGONAPI})
message(STATUS "LIMBO_ROOT_DIR = ${LIMBO_ROOT_DIR}")
message(STATUS "LIMBO_LIBRARIES = ${LIMBO_LIBRARIES}")

# Find the directory for LEMON. If cache variable LEMON_DIR is defined, use it as the path to LEMON, otherwise use the system LEMON_DIR
if (LEMON_DIR)
    set(LEMON_ROOT_DIR ${LEMON_DIR})
    message(STATUS "Use LEMON: ${LEMON_ROOT_DIR}")
else()
    set(LEMON_ROOT_DIR $ENV{LEMON_DIR})
    message(STATUS "Use system LEMON ${LEMON_ROOT_DIR}")
endif()
unset (LEMON_DIR CACHE)
find_library(LEMON_LIBRARIES
    NAMES emon
    HINTS ${LEMON_ROOT_DIR}/lib
    )
message(STATUS "LEMON_ROOT_DIR = ${LEMON_ROOT_DIR}")
message(STATUS "LEMON_LIBRARIES = ${LEMON_LIBRARIES}")

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

# Find the directory for GUROBI. If cache variable GUROBI_HOME is defined, use it as the path to GUROBI, otherwise use the system GUROBI_HOME
set(USE_GUROBI FALSE)
if (GUROBI_HOME)
    set(GUROBI_ROOT_DIR ${GUROBI_HOME})
    message(STATUS "Use GUROBI: ${GUROBI_ROOT_DIR}")
	set(USE_GUROBI TRUE)
elseif(DEFINED ENV{GUROBI_HOME})
    set(GUROBI_ROOT_DIR $ENV{GUROBI_HOME})
    message(STATUS "Use system GUROBI ${GUROBI_ROOT_DIR}")
	set(USE_GUROBI TRUE)
else()
	message(STATUS "No Gurobi")
endif()
unset (GUROBI_HOME CACHE)
if(USE_GUROBI)
	find_library(GUROBI_CPP
	    NAMES gurobi_g++5.2
	    HINTS ${GUROBI_ROOT_DIR}/lib
	    )
	find_library(GUROBI_C
	    NAMES gurobi80
	    HINTS ${GUROBI_ROOT_DIR}/lib
	    )
	set(GUROBI_LIBRARIES ${GUROBI_CPP} ${GUROBI_C})
	message(STATUS "GUROBI_ROOT_DIR = ${GUROBI_ROOT_DIR}")
	message(STATUS "GUROBI_LIBRARIES = ${GUROBI_LIBRARIES}")
endif()

find_package(Boost 1.6 COMPONENTS timer system graph iostreams)
find_package( ZLIB REQUIRED )

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

    install(DIRECTORY ${DOCUMENT_OUTPUT_PATH}/html DESTINATION share/doc)
endif()
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

#file(GLOB SOURCES cpp/*.cpp cpp/*h)
set(WELL_GEN_SOURCES cpp/WellGen.cpp cpp/WellGen.h)
set(LEGALIZE_SOURCES cpp/Legalize.cpp cpp/Legalize.h)

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
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -march=native -fopenmp -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fno-inline")
set(CMAKE_CXX_FLAGS_RELEASE "-O3") 
set(CMAKE_CXX_FLAGS_PROFILE "-O1 -pg -Winline")
set(CMAKE_EXE_LINKER_FLAGS "-std=c++11 -pthread")
#add_compile_options(-std=c++11)

set(WELL_GEN_NAME "well_gen")
set(LEGALIZE_NAME "legalize")
add_executable(${WELL_GEN_NAME} ${WELL_GEN_SOURCES})
add_executable(${LEGALIZE_NAME} ${LEGALIZE_SOURCES})

if(USE_GUROBI)
	include_directories(${INCLUDE_ROOT_DIR}
	    ${Boost_INCLUDE_DIR}
	    ${ZLIB_INCLUDE_DIRS}
	    ${LIMBO_ROOT_DIR}/include
	    ${LEMON_ROOT_DIR}/include
	    ${WNLIB_ROOT_DIR}/acc/h
	    ${LPSOLVE_ROOT_DIR} 
	    ${GUROBI_ROOT_DIR}/include
	                    )
else()
	include_directories(${INCLUDE_ROOT_DIR}
	    ${Boost_INCLUDE_DIR}
	    ${ZLIB_INCLUDE_DIRS}
	    ${LIMBO_ROOT_DIR}/include
	    ${LEMON_ROOT_DIR}/include
	    ${WNLIB_ROOT_DIR}/acc/h
	    ${LPSOLVE_ROOT_DIR} 
	                    )
endif()

target_link_libraries(${WELL_GEN_NAME} 
    ${LIMBO_LIBRARIES}
    ${LEMON_LIBRARIES}
    ${WNLIB_LIBRARIES}
    ${LPSOLVE_LIBRARIES}
    ${Boost_LIBRARIES} 
    ${ZLIB_LIBRARIES})


if(USE_GUROBI)
	target_link_libraries(${LEGALIZE_NAME} 
	    ${LIMBO_LIBRARIES}
	    ${LEMON_LIBRARIES}
	    ${WNLIB_LIBRARIES}
	    ${LPSOLVE_LIBRARIES}
	    ${Boost_LIBRARIES} 
	    ${ZLIB_LIBRARIES}
	    ${GUROBI_LIBRARIES})
	
	add_definitions(-DUSE_GUROBI)
else()
	target_link_libraries(${LEGALIZE_NAME} 
	    ${LIMBO_LIBRARIES}
	    ${LEMON_LIBRARIES}
	    ${WNLIB_LIBRARIES}
	    ${LPSOLVE_LIBRARIES}
	    ${Boost_LIBRARIES} 
	    ${ZLIB_LIBRARIES})
endif()
