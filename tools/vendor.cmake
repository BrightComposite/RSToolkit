#--------------------------------------------------------
#	ASD cmake vendor facilities
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

if(NOT ";${GUARD_BLOCKS};" MATCHES ";VENDOR_TOOL_GUARD;")
	set(GUARD_BLOCKS ${GUARD_BLOCKS};VENDOR_TOOL_GUARD CACHE INTERNAL "Guard blocks" FORCE)

    function(vendor VENDOR_KEY)
        if(EXISTS ${WORKSPACE_VENDOR_ROOT}/${VENDOR_KEY})
			set(VENDOR_DIR ${WORKSPACE_VENDOR_ROOT}/${VENDOR_KEY})
		elseif(EXISTS ${VENDOR_ROOT}/${VENDOR_KEY})
			set(VENDOR_DIR ${VENDOR_ROOT}/${VENDOR_KEY})
		elseif(EXISTS "${WORKSPACE_VENDOR_ROOT}/${VENDOR_KEY}.tar.gz")
			set(VENDOR_ARCHIVE_DIR ${WORKSPACE_VENDOR_ROOT})
			set(VENDOR_DIR ${WORKSPACE_VENDOR_ROOT}/${VENDOR_KEY})
			set(VENDOR_ARCHIVE 1)
		elseif(EXISTS "${VENDOR_ROOT}/${VENDOR_KEY}.tar.gz")
			set(VENDOR_ARCHIVE_DIR ${VENDOR_ROOT})
			set(VENDOR_DIR ${VENDOR_ROOT}/${VENDOR_KEY})
			set(VENDOR_ARCHIVE 1)
		elseif(EXISTS "${WORKSPACE_VENDOR_ROOT}/${VENDOR_KEY}.tar.gz.aa")
			set(VENDOR_ARCHIVE_DIR ${WORKSPACE_VENDOR_ROOT})
			set(VENDOR_DIR ${WORKSPACE_VENDOR_ROOT}/${VENDOR_KEY})
			set(VENDOR_ARCHIVE 1)
			set(VENDOR_ARCHIVE_SPLIT 1)
		elseif(EXISTS "${VENDOR_ROOT}/${VENDOR_KEY}.tar.gz.aa")
			set(VENDOR_ARCHIVE_DIR ${VENDOR_ROOT})
			set(VENDOR_DIR ${VENDOR_ROOT}/${VENDOR_KEY})
			set(VENDOR_ARCHIVE 1)
			set(VENDOR_ARCHIVE_SPLIT 1)
		else()
			message(FATAL_ERROR "Can't find vendor library '${VENDOR_KEY}'!")
		endif()

		if(NOT "${VENDOR_ARCHIVE}" STREQUAL "")
		    if(NOT "${VENDOR_ARCHIVE_SPLIT}" STREQUAL "")
                message(STATUS "Extract vendor library '${VENDOR_KEY}' from ${VENDOR_KEY}.tar.gz.*")

                if(WIN32)
                    execute_process(COMMAND copy /b ${VENDOR_KEY}.tar.gz.* ${VENDOR_KEY}.cat.tar.gz WORKING_DIRECTORY "${VENDOR_ARCHIVE_DIR}" RESULT_VARIABLE RESULT)
                elseif(UNIX)
                    execute_process(COMMAND sh -c "rm -f ${VENDOR_KEY}.cat.tar.gz" WORKING_DIRECTORY "${VENDOR_ARCHIVE_DIR}" RESULT_VARIABLE RESULT)
                    execute_process(COMMAND sh -c "cat ${VENDOR_KEY}.tar.gz.* >> ${VENDOR_KEY}.cat.tar.gz" WORKING_DIRECTORY "${VENDOR_ARCHIVE_DIR}" RESULT_VARIABLE RESULT)
                endif()

                if(NOT ${RESULT} EQUAL 0)
                    message(FATAL_ERROR "Can't concat archive!")
                endif()
                
                execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz "${VENDOR_KEY}.cat.tar.gz" WORKING_DIRECTORY "${VENDOR_ARCHIVE_DIR}" RESULT_VARIABLE RESULT)
            else()
                message(STATUS "Extract vendor library '${VENDOR_KEY}' from ${VENDOR_KEY}.tar.gz")
                execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz "${VENDOR_KEY}.tar.gz" WORKING_DIRECTORY "${VENDOR_ARCHIVE_DIR}" RESULT_VARIABLE RESULT)
        endif()

            if(NOT ${RESULT} EQUAL 0)
                message(FATAL_ERROR "Can't extract archive!")
            endif()
            
			if(NOT EXISTS ${VENDOR_DIR})
				message(FATAL_ERROR "Can't extract the archive with vendor library '${VENDOR_KEY}'!")
			endif()
		endif()

		set(ARGUMENTS ${ARGN})

        include(${VENDOR_DIR}/setup.cmake)
		message("${MESSAGES_INDENTATION}	* Use vendor library at ${VENDOR_DIR}")
    endfunction()
endif()