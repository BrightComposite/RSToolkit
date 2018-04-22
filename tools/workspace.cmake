#--------------------------------------------------------
#	ASD workspace configuration
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

set(GUARD_BLOCKS CACHE INTERNAL "Guard blocks" FORCE)
set(MESSAGES_INDENTATION "" CACHE STRING "Messages indentation" FORCE)

if ("${WORKSPACE_DIR}" STREQUAL "")
	set(WORKSPACE_ROOT ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Workspace root directory" FORCE)
else()
	set(WORKSPACE_ROOT ${WORKSPACE_DIR} CACHE PATH "Workspace root directory" FORCE)
endif()

if ("${ASD_TOOLS}" STREQUAL "")
	set(ASD_TOOLS ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "ASD tools directory" FORCE)
else()
	set(ASD_TOOLS ${ASD_TOOLS} CACHE PATH "ASD tools directory" FORCE)
endif()

if ("${ASD_ROOT}" STREQUAL "")
	get_filename_component(ASD_ROOT ${ASD_TOOLS}/.. ABSOLUTE)
	set(ASD_ROOT ${ASD_ROOT} CACHE PATH "ASD toolkit root directory" FORCE)
else()
	set(ASD_ROOT ${ASD_ROOT} CACHE PATH "ASD tools directory" FORCE)
endif()

include(${ASD_TOOLS}/utils.cmake)
include(${ASD_TOOLS}/config.cmake)

#--------------------------------------------------------
