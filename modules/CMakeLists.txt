#--------------------------------------------------------
#	Rapture State Toolkit
#--------------------------------------------------------

project(RaptureStateToolkit CXX)

#--------------------------------------------------------

cmake_minimum_required(VERSION 3.0)

#--------------------------------------------------------

get_filename_component(RAPTURE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/.. REALPATH)
set(RAPTURE_TOOLS ${RAPTURE_ROOT}/tools)

set(GUARD_BLOCKS CACHE INTERNAL "Guard blocks" FORCE)
set(COMPILE_OPTIONS "/arch:AVX2" CACHE INTERNAL "Compile options" FORCE)

add_subdirectory(tests/core-test)
add_subdirectory(tests/d3d-test)
add_subdirectory(tests/ui-test)

add_subdirectory(projects/independence)
add_subdirectory(projects/InTheMoonlight)

#--------------------------------------------------------