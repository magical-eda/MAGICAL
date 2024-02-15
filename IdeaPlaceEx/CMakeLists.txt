cmake_minimum_required(VERSION 3.13)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON )
set(CMAKE_VERBOSE_MAKEFILE ON)

set(PROJECT_NAME "IdeaPlaceEx")
project(${PROJECT_NAME})



# deal with GCC dual ABI issue 
if(NOT CMAKE_CXX_ABI)
    set(CMAKE_CXX_ABI 0 CACHE STRING
        "Choose the value for _GLIBCXX_USE_CXX11_ABI, options are: 0|1."
        FORCE)
endif(NOT CMAKE_CXX_ABI)
message(STATUS "CMAKE_CXX_ABI: _GLIBCXX_USE_CXX11_ABI=${CMAKE_CXX_ABI}")
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=${CMAKE_CXX_ABI})

find_package (PkgConfig)

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

find_package(Boost 1.6 COMPONENTS system graph iostreams)
find_package( ZLIB REQUIRED )



# Find Lemon
if (LEMON_DIR)
  set(LEMON_ROOT_DIR ${LEMON_DIR})
else()
  set(LEMON_ROOT_DIR $ENV{LEMON_DIR})
endif()
message(STATUS "Find Lemon: ${LEMON_ROOT_DIR}")
unset(LEMON_DIR CACHE)



# Find Eigen
if (EIGEN_DIR)
    set(EIGEN_ROOT_DIR ${EIGEN_DIR})
else()
    set(EIGEN_ROOT_DIR $ENV{EIGEN_INC})
endif()
message(STATUS "Find Eigen: ${EIGEN_ROOT_DIR}")
unset(LEMON_DIR CACHE)


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



file(GLOB SOURCES src/global/*.h    src/global/*.cpp
                  src/db/*.h        src/db/*.cpp
                  src/util/*.h      src/util/*.cpp 
                  src/parser/*.h    src/parser/*.cpp
                  src/writer/gdsii/*.h    src/writer/gdsii/*.cpp
                  src/place/*.h    src/place/*.cpp src/place/nlp/*.cpp
                  src/pinassign/*.h src/pinassign/*.cpp
                  src/main/IdeaPlaceEx.h src/main/IdeaPlaceEx.cpp)

file(GLOB EXE_SOURCES src/main/main.cpp)
file(GLOB PY_API_SOURCES src/api/*.cpp)

# linking libraries
file(GLOB STATIC_LIB 
    ${LIMBO_ROOT_DIR}/lib/libgdsdb.a 
    ${LIMBO_ROOT_DIR}/lib/libgdsparser.a
    ${LIMBO_ROOT_DIR}/lib/libgzstream.a
    )

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

#wnlib: conjugate gradient for reference
if (WNLIB_DIR)
    set(WNLIB_ROOT_DIR ${WNLIB_DIR})
    message(STATUS "Use wnlib ${WNLIB_ROOT_DIR}")
    set(USE_WNLIB TRUE)
elseif(DEFINED ENV{WNLIB_DIR})
    set(WNLIB_ROOT_DIR $ENV{WNLIB_DIR})
    message(STATUS "Use system wnlib ${WNLIB_ROOT_DIR}")
    set(USE_WNLIB TRUE)
else()
    message(STATUS "Disable wnlib")
    set(USE_WNLIB FALSE)
endif()
unset(WNLIB_DIR CACHE)

if (USE_WNLIB)
    find_library(WNLIB_LIBRARIES
        NAMES text.a
        HINTS ${WNLIB_ROOT_DIR}/acc
        )
endif()



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
set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -fopenmp ")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fno-inline ")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_PROFILE "-O3 -pg -Winline")
set(CMAKE_EXE_LINKER_FLAGS "-pthread -static-libgcc -static-libstdc++")


set(TO_LINK_LIBS ${STATIC_LIB} ${Boost_LIBRARIES} ${ZLIB_LIBRARIES})


set(TO_INCLUDE_PATH 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
    ${LIMBO_ROOT_DIR}/include
    ${PYTHON_INCLUDE_DIRS}
    ${PYBIND11_ROOT_DIR}/include
    ${LEMON_ROOT_DIR}/include
    ${EIGEN_ROOT_DIR}
)

if (USE_LPSOLVE)
    set(TO_LINK_LIBS ${TO_LINK_LIBS} ${LPSOLVE_LIBRARIES})
    set(TO_INCLUDE_PATH ${TO_INCLUDE_PATH} ${LPSOLVE_ROOT_DIR})
endif()

if (USE_GUROBI)
    set(TO_LINK_LIBS ${TO_LINK_LIBS} ${GUROBI_LIBRARIES})
    set(TO_INCLUDE_PATH ${TO_INCLUDE_PATH} ${GUROBI_ROOT_DIR}/include)
endif()

if (USE_WNLIB)
    set(TO_LINK_LIBS ${TO_LINK_LIBS} ${WNLIB_LIBRARIES})
    set(TO_INCLUDE_PATH ${TO_INCLUDE_PATH} ${WNLIB_ROOT_DIR}/acc/h)
endif()

add_executable(${PROJECT_NAME} ${SOURCES} ${EXE_SOURCES})

include_directories(${TO_INCLUDE_PATH})


target_link_libraries(${PROJECT_NAME} 
    ${TO_LINK_LIBS}
    )

## Add modules to pybind
pybind11_add_module("IdeaPlaceExPy" ${PY_API_SOURCES} ${SOURCES})
target_link_libraries("IdeaPlaceExPy" PUBLIC ${TO_LINK_LIBS}
)

##Install
#install (TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX_DIR}/bin)
