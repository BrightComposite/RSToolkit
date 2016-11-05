#--------------------------------------------------------
#	ASD global configuration
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.0)

#--------------------------------------------------------

set(GUARD_BLOCKS CACHE INTERNAL "Guard blocks" FORCE)
set(MESSAGES_INDENTATION "" CACHE STRING "Messages indentation" FORCE)

set(WORKSPACE_ROOT ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Workspace root directory")

set(ASD_TOOLS ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "ASD tools directory")
get_filename_component(ASD_ROOT ${ASD_TOOLS}/.. ABSOLUTE)
set(ASD_ROOT ${ASD_ROOT} CACHE PATH "ASD toolkit root directory")

message("ASD Toolkit at ${ASD_ROOT}")

include(${ASD_TOOLS}/module.cmake)
include(${ASD_TOOLS}/config.cmake)

#--------------------------------------------------------
