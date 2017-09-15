#--------------------------------------------------------
#	ASD global configuration
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

set(GLOBAL_REINSTALL 0 CACHE BOOL "Reinstall flag" FORCE)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "")

set(CMAKE_SUPPRESS_REGENERATION true)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DASD_DEBUG=1")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DASD_DEBUG=0")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -DASD_DEBUG=0")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DASD_DEBUG=0")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_UNICODE -DUNICODE -DNOMINMAX")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	if(UNIX)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -pthread")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
	else()
		add_compile_options($<$<CONFIG:RELASE>:-O2> -Wall -std=c++14 -pthread)
	endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()

# SIMD

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SIMD_LEVEL=SSE3")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	#add_compile_options(/arch:AVX2)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	if(UNIX)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse3")
	else()
		add_compile_options(-msse3)
	endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse3")
endif()

#if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	#set(USE_DIRECT3D 1 CACHE INTERNAL "Use direct3d flag")
    #add_module(d3d tests)
#else()
	set(USE_DIRECT3D 0 CACHE INTERNAL "Use direct3d flag")
#endif()

#--------------------------------------------------------
