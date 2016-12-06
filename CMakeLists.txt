#--------------------------------------------------------
#	asd
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

project(asd VERSION 0.1)

#--------------------------------------------------------

include(tools/workspace.cmake)

add_module(ui-themes modules)

add_module(core tests)
add_module(math tests)
add_module(ui   tests)

#--------------------------------------------------------
