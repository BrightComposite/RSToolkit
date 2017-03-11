#--------------------------------------------------------
#	ASD global configuration
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "")

set(CMAKE_SUPPRESS_REGENERATION true)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL MSVC)
	add_definitions(-D_UNICODE -DUNICODE -DNOMINMAX)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL GNU)
    add_compile_options(-std=c++14 -pthread -fabi-version=10)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL Clang)
    set(CMAKE_CXX_FLAGS -Wall -std=c++14)
endif()

# SIMD

add_definitions(-D_SIMD_LEVEL=SSE3)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL MSVC)
	#add_compile_options(/arch:AVX2)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL GNU)
	add_compile_options(-msse3)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL Clang)
	set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -msse3)
endif()

#if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	#set(USE_DIRECT3D 1 CACHE INTERNAL "Use direct3d flag")
    #add_module(d3d tests)
#else()
	set(USE_DIRECT3D 0 CACHE INTERNAL "Use direct3d flag")
#endif()

#add_definitions(-DUSE_AVX)

#--------------------------------------------------------
