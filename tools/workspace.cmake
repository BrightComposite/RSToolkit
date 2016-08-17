#--------------------------------------------------------
#	Rapture State global configuration
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.0)

#--------------------------------------------------------

set(GUARD_BLOCKS CACHE INTERNAL "Guard blocks" FORCE)
set(MESSAGES_INDENTATION "" CACHE STRING "Messages indentation" FORCE)

set(WORKSPACE_ROOT ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Workspace root directory")

set(RAPTURE_TOOLS ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "Rapture tools directory")
get_filename_component(RAPTURE_ROOT ${RAPTURE_TOOLS}/.. ABSOLUTE)
set(RAPTURE_ROOT ${RAPTURE_ROOT} CACHE PATH "Rapture state toolkit root directory")

message("Rapture State Toolkit at ${RAPTURE_ROOT}")

include(${RAPTURE_TOOLS}/module.cmake)
include(${RAPTURE_TOOLS}/config.cmake)

#--------------------------------------------------------
