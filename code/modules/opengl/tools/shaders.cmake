#--------------------------------------------------------
#	ASD cmake shader facilities
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.0)

#--------------------------------------------------------

set(TOOLS_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(shaders)
    collect_files(shaders_list ${ARGN})

    foreach(shader ${shaders_list})
        get_filename_component(FileName ${shader} NAME_WE)
        get_filename_component(FileDir ${shader} DIRECTORY)

            # Input shader
        set(InputFile ${FileDir}/${FileName}.glsl)

        set(OutputFile ${FileDir}/${FileName}.shader.h) # Output .shader.h file

        # Get type of shader to compile (vs, ps/fs)

        string(LENGTH ${FileName} FileLen)
        math(EXPR FileLen "${FileLen} - 2")
        string(SUBSTRING ${FileName} ${FileLen} -1 ShaderType)
        string(TOLOWER ${ShaderType} ShaderType)

        # Add the command that will process each single file

        if("${CURRENT_SOURCE_GROUP_PATH}" STREQUAL "")
            string(REPLACE "${PROJECT_SOURCE_DIR}" "" ShaderPath ${FileDir})
        else()
            string(REPLACE "${PROJECT_SOURCE_DIR}/${CURRENT_SOURCE_GROUP_PATH}" "" ShaderPath ${FileDir})
        endif()

        string(REGEX REPLACE "^[\\/]+" "" ShaderPath ${ShaderPath})

        if(NOT "${CURRENT_SOURCE_DOMAIN}" STREQUAL "")
            string(REPLACE "${CURRENT_SOURCE_DOMAIN}/" "" ShaderId ${ShaderPath})
        endif()

        string(REGEX REPLACE "[\\/]" _ ShaderGroup ${ShaderId})
        string(REGEX REPLACE "[^a-zA-Z0-9_]" "" ShaderGroup ${ShaderGroup})

        set(OutputVariable shader_code_${ShaderGroup})

        if(NOT ";${CURRENT_SHADERS_LIST};" MATCHES ";${ShaderGroup};")
            set(CURRENT_SHADERS_LIST ${CURRENT_SHADERS_LIST};${ShaderGroup} CACHE INTERNAL "" FORCE)
            set(${ShaderGroup}_id ${ShaderId} CACHE INTERNAL "" FORCE)
            set(${ShaderGroup}_path ${ShaderPath} CACHE INTERNAL "" FORCE)
        endif()

        set(${ShaderGroup}_types ${${ShaderGroup}_types} ${ShaderType} CACHE INTERNAL "" FORCE)

        add_custom_command(
            OUTPUT ${OutputFile}
            COMMAND call ${CMAKE_COMMAND} -D ASD_ROOT=${ASD_ROOT} -D Input=${InputFile} -D Output=${OutputFile} -D ShaderType=${ShaderType} -D OutputVariable=${OutputVariable} -P "${TOOLS_DIR}/compile-shader.cmake"
            MAIN_DEPENDENCY ${InputFile}
            COMMENT ""
            WORKING_DIRECTORY ${TOOLS_DIR}
            VERBATIM
            )
    endforeach()
endfunction()

function(shaders_output out)
    set(TEMPLATE_FILE "${TOOLS_DIR}/templates/fx_include.h")
    set(HEADER_FILE   "${TOOLS_DIR}/templates/fx_header.h")
    set(INCLUDES)

    foreach(ShaderGroup ${CURRENT_SHADERS_LIST})
        set(ShaderPath ${${ShaderGroup}_path})
        set(ShaderId ${${ShaderGroup}_id})

        set(ShaderCodeUnits)

        foreach(ShaderType ${${ShaderGroup}_types})
            if("${ShaderType}" STREQUAL "vs")
                set(SHADER_TYPE RFX_VERTEX_SHADER)
            elseif("${ShaderType}" STREQUAL "ps")
                set(SHADER_TYPE RFX_FRAGMENT_SHADER)
            elseif("${ShaderType}" STREQUAL "gs")
                set(SHADER_TYPE RFX_GEOMETRY_SHADER)
            endif()

            if("${INCLUDES}" STREQUAL "")
                set(INCLUDES "#include <${ShaderPath}/${ShaderType}.shader.h>")
            else()
                set(INCLUDES "${INCLUDES}\n#include <${ShaderPath}/${ShaderType}.shader.h>")
            endif()

            if("${ShaderCodeUnits}" STREQUAL "")
                set(ShaderCodeUnits "{shader_code_${ShaderGroup}_${ShaderType}, ${SHADER_TYPE}}")
            else()
                set(ShaderCodeUnits "${ShaderCodeUnits}, {shader_code_${ShaderGroup}_${ShaderType}, ${SHADER_TYPE}}")
            endif()
        endforeach()

        set(${ShaderGroup}_types CACHE INTERNAL "" FORCE)
        set(ShaderCodeUnits "\tstatic const RawShaderCodeUnit shader_code_${ShaderGroup}[]{${ShaderCodeUnits}};\n")

        if("${CONTENTS}" STREQUAL "")
            set(CONTENTS "${ShaderCodeUnits}")
        else()
            set(CONTENTS "${CONTENTS}${ShaderCodeUnits}")
        endif()

        set(ShaderSet "\t\t{\"${ShaderId}\", shader_code_${ShaderGroup}_layout, shader_code_${ShaderGroup}}")

        if("${ShaderSetArray}" STREQUAL "")
            set(ShaderSetArray "${ShaderSet}")
        else()
            set(ShaderSetArray "${ShaderSetArray},\n${ShaderSet}")
        endif()
    endforeach()

    file(READ ${HEADER_FILE} HEADER)
    set(CONTENTS "${HEADER}\n${CONTENTS}\n\tstatic const RawShaderCode shaders[] {\n${ShaderSetArray}\n\t};")
    set(HEADER)

    collect_files(outfile ${out})
    configure_file(${TEMPLATE_FILE} ${outfile})
    set(CURRENT_SHADERS_LIST CACHE INTERNAL "" FORCE)
endfunction()