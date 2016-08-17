#--------------------------------------------------------
#	Rapture State Toolkit
#--------------------------------------------------------

project(RaptureStateToolkit CXX)

cmake_minimum_required(VERSION 3.0)

#--------------------------------------------------------

include(tools/workspace.cmake)

add_module(core tests)
add_module(math tests)
add_module(ui   tests)

#--------------------------------------------------------
