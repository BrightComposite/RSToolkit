#--------------------------------------------------------
#	Rapture State Toolkit
#--------------------------------------------------------

project(RaptureStateToolkit CXX)

#--------------------------------------------------------

cmake_minimum_required(VERSION 3.0)

#--------------------------------------------------------

set(GUARD_BLOCKS CACHE INTERNAL "Guard blocks" FORCE)
set(DEPENDENCIES_LIST_INDENTATION CACHE STRING "Dependency list indentation" FORCE)

get_filename_component(RAPTURE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/.. ABSOLUTE)
set(RAPTURE_ROOT ${RAPTURE_ROOT} CACHE PATH "Rapture state engine root directory")
set(RAPTURE_TOOLS ${RAPTURE_ROOT}/tools CACHE PATH "Rapture tools directory")

include(${RAPTURE_TOOLS}/module.cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "")

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_compile_options(/arch:AVX2)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    add_compile_options(-std=c++14 -pthread -mavx2 -fabi-version=10)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    set(CMAKE_CXX_FLAGS -Wall -std=c++14)
endif()

add_definitions(-DUSE_AVX)

add_module(tests/core-test ../)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_module(tests/d3d-test ../)
endif()

add_module(tests/ui-test ../)
add_module(tests/small-test ../)

add_module(projects/independence ../)
add_module(projects/BasicGame ../)

#--------------------------------------------------------
