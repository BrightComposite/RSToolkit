#--------------------------------------------------------
#	ASD cmake git facilities
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

if(NOT ";${GUARD_BLOCKS};" MATCHES ";GIT_TOOL_GUARD;")
	set(GUARD_BLOCKS ${GUARD_BLOCKS};GIT_TOOL_GUARD CACHE INTERNAL "Guard blocks" FORCE)
	
	function(git_clone REPO DESTINATION WORKING_DIR)
		execute_process(COMMAND git clone "${REPO}" "${DESTINATION}" WORKING_DIRECTORY "${WORKING_DIR}")
	endfunction()
endif()