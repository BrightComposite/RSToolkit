#--------------------------------------------------------
#	asd
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

project(asd VERSION 0.1)

#--------------------------------------------------------

include(tools/workspace.cmake)
include(tools/module.cmake)

add_module(ui-themes)

add_test(core)
add_test(math)
add_test(ui)
add_test(message)
add_test(handle)
add_test(aligned)
add_test(space)
add_test(glx)

#add_command(check)
