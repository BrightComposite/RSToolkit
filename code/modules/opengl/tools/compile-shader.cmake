#--------------------------------------------------------
#	ASD cmake shader compiler wrapper
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.0)

#--------------------------------------------------------

get_filename_component(OutputDir ${Output} DIRECTORY)
get_filename_component(OutputName ${Output} NAME_WE)

set(TimestampFile ${OutputDir}/${OutputName}.timestamp)
file(TIMESTAMP ${Input} Timestamp)

if(NOT EXISTS ${Output})
    if(NOT EXISTS ${OutputDir})
        file(MAKE_DIRECTORY ${OutputDir})
    endif()

    file(WRITE ${Output})
elseif(EXISTS ${TimestampFile})
    file(READ ${TimestampFile} SavedTimestamp)

    if(${SavedTimestamp} STREQUAL ${Timestamp})
        return()
    endif()
endif()

set(VARIABLE ${OutputVariable}_${ShaderType})

file(READ ${Input} CONTENTS)
set(INCLUDE "#include <opengl/vertex_layout.h>")

if("${ShaderType}" STREQUAL "vs")
    string(REGEX MATCH "!vertex:[ a-z2-4-]*" layout ${CONTENTS})

    if(NOT "${layout}" STREQUAL "")
        string(REGEX REPLACE "!vertex:[ ]*" "" layout ${layout})
        string(REGEX REPLACE " " "" layout ${layout})

        set(LAYOUT "static const ::asd::opengl::vertex_layout & ${OutputVariable}_layout = ::asd::opengl::vertex_layouts::${layout}::get();\n")
    else()
        set(LAYOUT "#error \"The vertex layout must be declared in the shader code! Example: /* !vertex: p3 c4 */\"\n")
    endif()
    
    configure_file("${CMAKE_CURRENT_LIST_DIR}/templates/shader.vs.h" ${Output})
else()
    configure_file("${CMAKE_CURRENT_LIST_DIR}/templates/shader.h" ${Output})
endif()

message("embedded glsl shader save succeeded; see ${Output}")

file(WRITE ${TimestampFile} "${Timestamp}")

#--------------------------------------------------------
