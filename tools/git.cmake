#--------------------------------------------------------
#	ASD cmake git facilities
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

if(NOT ";${GUARD_BLOCKS};" MATCHES ";GIT_TOOL_GUARD;")
    set(GUARD_BLOCKS ${GUARD_BLOCKS};GIT_TOOL_GUARD CACHE INTERNAL "Guard blocks" FORCE)
	
    function(git_clone REPO DESTINATION WORKING_DIR)
        cmake_parse_arguments(CLONE "" "TAG" "OPTIONS" ${ARGN})

        execute_process(COMMAND git clone ${CLONE_OPTIONS} "${REPO}" "${DESTINATION}" WORKING_DIRECTORY "${WORKING_DIR}")

        if (NOT "${CLONE_TAG}" STREQUAL "")
            execute_process(COMMAND git checkout "tags/${CLONE_TAG}" WORKING_DIRECTORY "${WORKING_DIR}/${DESTINATION}")
        endif()
    endfunction()
    
    function(git_submodule PATH SUBMODULE)
        execute_process(COMMAND git submodule update --init -- "./${SUBMODULE}" WORKING_DIRECTORY "${PATH}")
    endfunction()
endif()