cmake_minimum_required(VERSION 3.0)
#set(CMAKE_VERBOSE_MAKEFILE ON)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_NAME "anaroute")
project(${PROJECT_NAME})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

if (CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX_DIR ${CMAKE_INSTALL_PREFIX})
else()
  set(CMAKE_INSTALL_PREFIX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/install)
endif()
message(STATUS "Install Prefix: ${CMAKE_INSTALL_PREFIX}")
unset(CMAKE_INSTALL_PREFIX)

# Find LIMBO
if (LIMBO_DIR)
  set(LIMBO_ROOT_DIR ${LIMBO_DIR})
else()
  set(LIMBO_ROOT_DIR $ENV{LIMBO_DIR})
endif()
message(STATUS "Find Limbo: ${LIMBO_ROOT_DIR}")
unset(LIMBO_DIR CACHE)

# Find Lemon
if (LEMON_DIR)
  set(LEMON_ROOT_DIR ${LEMON_DIR})
else()
  set(LEMON_ROOT_DIR $ENV{LEMON_DIR})
endif()
message(STATUS "Find Lemon: ${LEMON_ROOT_DIR}")
unset(LEMON_DIR CACHE)


# Find SparseHash
if (SPARSE_HASH_DIR)
  set(SPARSEHASH_ROOT_DIR ${SPARSE_HASH_DIR})
else()
  set(SPARSEHASH_ROOT_DIR $ENV{SPARSE_HASH_DIR})
endif()
message(STATUS "Find SparseHash: ${SPARSEHASH_ROOT_DIR}")
unset(SPARSE_HASH_DIR CACHE)

# Find Boost
#if (BOOST_DIR)
  #set(BOOST_ROOT ${BOOST_DIR})
  #set(BOOST_INCLUDEDIR ${BOOST_DIR}/boost)
  #set(BOOST_LIBRARYDIR ${BOOST_DIR}/libs)
#else()
  #set(BOOST_ROOT $ENV{BOOST_DIR})
  #set(BOOST_INCLUDEDIR $ENV{BOOST_DIR}/boost)
  #set(BOOST_LIBRARYDIR $ENV{BOOST_DIR}/libs)
#endif()

find_package(Boost 1.6 REQUIRED COMPONENTS system graph iostreams)

# Find Zlib
find_package(ZLIB REQUIRED)

# Find pybind11
if (PYBIND11_DIR)
  set(PYBIND11_ROOT_DIR ${PYBIND11_DIR})
else()
  set(PYBIND11_ROOT_DIR $ENV{PYBIND11_DIR})
endif()
message(STATUS "Find Pybind11: ${PYBIND11_ROOT_DIR}")
unset(PYBIND11_DIR)
add_subdirectory(${PYBIND11_ROOT_DIR} "./pybind11")
set(PYBIND11_CPP_STANDARD -std=c++14)

# CXX flags
set(CMAKE_CXX_FLAGS "-g -std=c++14 -Wall -fPIC")
set(CMAKE_CXX_FLAGS "-O3 -std=c++14 -Wall -fPIC")

##################################################
#        Source Code                             #
##################################################
set(SRC
  src/util/util.cpp
  src/util/timeUsage.cpp
  src/db/lef/layer/lefCutLayer.cpp
  src/db/lef/layer/lefRoutingLayer.cpp
  src/db/lef/lefSite.cpp
  src/db/lef/lefVia.cpp
  src/db/dbLef.cpp
  src/db/dbTechfile.cpp
  src/db/dbPin.cpp
  src/db/dbCir.cpp
  src/db/dbNet.cpp
  src/parser/parser.cpp
  src/parser/parLef.cpp
  src/parser/parTech.cpp
  src/parser/parIspd08.cpp
  src/parser/parGds.cpp
  src/parser/parSymNet.cpp
  src/parser/parIOPin.cpp
  src/gr/grMgr.cpp
  src/gr/grGridRoute.cpp
  src/gr/grAstar.cpp
  src/ta/taMgr.cpp
  src/dr/drMgr.cpp
  src/dr/drGridlessRoute.cpp
  src/dr/drAstar.cpp
  src/dr/drViaSelector.cpp
  src/drc/drcMgr.cpp
  src/writer/writer.cpp
  src/writer/wrLayout.cpp
  src/writer/wrGrGuide.cpp
  src/writer/wrGds.cpp
  src/writer/wrDumb.cpp
)

set(SRC_EXE
  src/main/main.cpp
  src/main/anaroute.cpp
)

set(SRC_API
  src/api/apiPy.cpp  
)

add_executable(${PROJECT_NAME} ${SRC} ${SRC_EXE})
pybind11_add_module(${PROJECT_NAME}Py ${SRC} ${SRC_API})

##################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${ZLIB_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${SPARSEHASH_ROOT_DIR}/include
  ${LIMBO_ROOT_DIR}/include
  ${PYBIND11_ROOT_DIR}/include
  ${LEMON_ROOT_DIR}/include
)

file(GLOB LIMBO_LIB 
  #lefdef
  ${LIMBO_ROOT_DIR}/lib/libcdef.a 
  ${LIMBO_ROOT_DIR}/lib/libcdefzlib.a 
  ${LIMBO_ROOT_DIR}/lib/libdef.a 
  ${LIMBO_ROOT_DIR}/lib/libdefzlib.a 
	${LIMBO_ROOT_DIR}/lib/libdefparser.a
	${LIMBO_ROOT_DIR}/lib/libdefparseradapt.a
  ${LIMBO_ROOT_DIR}/lib/libclef.a 
  ${LIMBO_ROOT_DIR}/lib/libclefzlib.a 
  ${LIMBO_ROOT_DIR}/lib/liblef.a 
  ${LIMBO_ROOT_DIR}/lib/liblefzlib.a
	${LIMBO_ROOT_DIR}/lib/liblefparseradapt.a
  #limbo gdsparser
  ${LIMBO_ROOT_DIR}/lib/libgdsdb.a 
  ${LIMBO_ROOT_DIR}/lib/libgdsparser.a
)

target_link_libraries(${PROJECT_NAME} ${LIMBO_LIB} ${Boost_LIBRARIES} ${ZLIB_LIBRARIES})
target_link_libraries(${PROJECT_NAME}Py PUBLIC ${LIMBO_LIB} ${Boost_LIBRARIES} ${ZLIB_LIBRARIES})

#Install
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX_DIR}/bin)

