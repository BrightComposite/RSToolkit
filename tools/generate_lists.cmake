#--------------------------------------------------------
#	Rapture State CmakeLists generator
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.0)

#--------------------------------------------------------

get_filename_component(PATH ${PATH} ABSOLUTE)
get_filename_component(WORKSPACE ${WORKSPACE} ABSOLUTE)
get_filename_component(RAPTURE_ROOT ${CMAKE_CURRENT_LIST_DIR}/.. ABSOLUTE)

set(OUTPUT ${WORKSPACE}/${DESTINATION})

message(STATUS "Module name: ${NAME}")
message(STATUS "Module type: ${TYPE}")
message(STATUS "Module path: ${PATH}")
message(STATUS "Source groups: ${GROUPS}")
message(STATUS "Output directory: ${OUTPUT}")

message(STATUS "Collecting files...")

set(CMAKE_LISTS_INDENTATION CACHE INTERNAL "")

#--------------------------------------------------------

function(collect OUT PATH)
    set(TEMP_INDENT ${CMAKE_LISTS_INDENTATION})
    set(CMAKE_LISTS_INDENTATION "${CMAKE_LISTS_INDENTATION}\t" CACHE INTERNAL "")

    file(GLOB ENTRIES RELATIVE ${PATH} "${PATH}/*")

    set(FILE_SOURCES)
    set(DIR_SOURCES)

    foreach(ENTRY ${ENTRIES})
        if(IS_DIRECTORY ${PATH}/${ENTRY})
            collect(SOURCES ${PATH}/${ENTRY})

            if(NOT "${SOURCES}" STREQUAL "")
                list(APPEND DIR_SOURCES ${CMAKE_LISTS_INDENTATION}${ENTRY}/)
                list(APPEND DIR_SOURCES ${SOURCES})
                list(APPEND DIR_SOURCES ${CMAKE_LISTS_INDENTATION}..)
            endif()
        else()
            list(APPEND FILE_SOURCES ${CMAKE_LISTS_INDENTATION}${ENTRY})
        endif()
    endforeach()

    set(${OUT} ${FILE_SOURCES} ${DIR_SOURCES} PARENT_SCOPE)

    set(CMAKE_LISTS_INDENTATION ${TEMP_INDENT} CACHE INTERNAL "")
endfunction()

#--------------------------------------------------------

set(OUTPUT_DIR ${OUTPUT}/${NAME})

file(MAKE_DIRECTORY ${OUTPUT_DIR})

foreach(ENTRY ${GROUPS})
    if(DEFINED GROUP_NAME)    
        collect(SOURCES ${PATH}/${ENTRY})
        
        if(NOT "${SOURCES}" STREQUAL "")
            file(MAKE_DIRECTORY ${OUTPUT_DIR}/${ENTRY})
            file(COPY ${PATH}/${ENTRY}/ DESTINATION ${OUTPUT_DIR}/${ENTRY}/)

            if(DEFINED library_sources)
                set(library_sources "${library_sources}\n\n")
            else()
                set(library_sources)
            endif()

            set(library_sources "${library_sources}\t\tgroup(${ENTRY} ${GROUP_NAME})\n")
            set(library_sources "${library_sources}\t\tfiles(\n")
                        
            foreach(F ${SOURCES})
                set(library_sources "${library_sources}\t\t${F}\n")
            endforeach()

            set(library_sources "${library_sources}\t\t)")
        endif()

        unset(GROUP_NAME)
    else()
        set(GROUP_NAME ${ENTRY})
    endif()
endforeach()

#--------------------------------------------------------

set(library_name ${NAME})
string(TOUPPER ${TYPE} library_type)
set(library_destination ${DESTINATION})

message(STATUS "Generating CmakeLists.txt...")

set(TEMPLATE_FILE ${RAPTURE_ROOT}/templates/module/generate/CMakeLists.txt)

if(EXISTS "${TEMPLATE_FILE}")
    configure_file(${TEMPLATE_FILE} ${OUTPUT_DIR}/CMakeLists.txt @ONLY)
else()
    message(FATAL_ERROR "Can't generate module \"${NAME}\". Can't find file ${TEMPLATE_FILE}")
endif()

message(STATUS "Congratulations! Module ${NAME} has been created! Output is placed to ${OUTPUT_DIR}")

unset(CMAKE_LISTS_INDENTATION CACHE)


#--------------------------------------------------------
