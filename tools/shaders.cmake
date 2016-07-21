cmake_minimum_required(VERSION 3.0)

function(escape_regular OUT_STR STR)
	string(REPLACE "\\" "\\\\" BUF "${STR}")
	string(REPLACE "/" "\\/" BUF "${BUF}")
	string(REPLACE "+" "\\+" BUF "${BUF}")
	string(REPLACE "." "\\." BUF "${BUF}")
	string(REPLACE ":" "\\:" BUF "${BUF}")

	set(${OUT_STR} ${BUF} PARENT_SCOPE)
endfunction()

if(WIN32)
	function(add_shaders SHADER_LANG RES_DIR RES_GROUP INC_DIR INC_GROUP SHADERS_ROOT OUTPUT_HEADER)
		set(ROOT_DIR ${PROJECT_SOURCE_DIR})

		set_module_sources(OUT_SHADERS_LIST
			SRC_GROUP ${RES_DIR} ${RES_GROUP}
				START_SECTION ${SHADERS_ROOT}
					${ARGN}
				END_SECTION
		)

		set(INCLUDE_ENTRIES)

		if("${SHADER_LANG}" STREQUAL "HLSL")
			foreach(src_entry ${ARGN})
				string(REGEX REPLACE "\\.fx$" ".inc" inc_entry ${src_entry})
				list(APPEND INCLUDE_ENTRIES ${inc_entry})
			endforeach()
		elseif("${SHADER_LANG}" STREQUAL "GLSL")
			foreach(src_entry ${ARGN})
				string(REGEX REPLACE "\\.glsl$" ".inc" inc_entry ${src_entry})
				list(APPEND INCLUDE_ENTRIES ${inc_entry})
			endforeach()
		endif()

		foreach(shader ${OUT_SHADERS_LIST})
			get_filename_component(FileName ${shader} NAME_WE)
			get_filename_component(FileDir ${shader} DIRECTORY)

			escape_regular(res_root ${ROOT_DIR}/${RES_DIR})
			string(REGEX REPLACE ${res_root} "" ShaderPath ${FileDir})

			if("${SHADER_LANG}" STREQUAL "HLSL")
				set(InputFile ${RES_DIR}${ShaderPath}/${FileName}.fx) # Input shader
			elseif("${SHADER_LANG}" STREQUAL "GLSL")
				set(InputFile ${RES_DIR}${ShaderPath}/${FileName}.glsl) # Input shader
			endif()
			
			set(OutputFile ${INC_DIR}${ShaderPath}/${FileName}.inc) # Output .inc file

			# Get type of shader to compile (vs, ps)

			string(LENGTH ${FileName} FileLen)
			math(EXPR FileLen "${FileLen} - 2")
			string(SUBSTRING ${FileName} ${FileLen} -1 ShaderType)
			string(TOLOWER ${ShaderType} ShaderType)

			# Add the command that will process each single file

			escape_regular(shaders_root ${SHADERS_ROOT})
			string(REGEX REPLACE ${shaders_root} "" ShaderPath ${ShaderPath})
			
			if("${ShaderPath}" MATCHES "^[\\/]+")
				string(REGEX REPLACE "^[\\/]+" "" ShaderPath ${ShaderPath})
			endif()

			string(REGEX REPLACE "[\\/]" _ ShaderId ${ShaderPath}/${FileName})
			string(REGEX REPLACE "[^a-zA-Z0-9_]" "" ShaderId ${ShaderId})

			set(OutputVariable shader_code_${ShaderId})
			
			add_custom_command(
				OUTPUT ${OutputFile}
				COMMAND call ${CMAKE_COMMAND} -D Input=${InputFile} -D Output=${OutputFile} -D ShaderType=${ShaderType} -D OutputVariable=${OutputVariable} -D ShaderLang=${SHADER_LANG} -P "${RAPTURE_TOOLS}/compile-shader.cmake"
				MAIN_DEPENDENCY ${InputFile}
				COMMENT ""
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				VERBATIM
				)
		endforeach()

		add_module_sources(
			SRC_GROUP ${INC_DIR} ${INC_GROUP}
				START_SECTION ${SHADERS_ROOT}
					${INCLUDE_ENTRIES}
				END_SECTION
		)

		set(${PROJECT_NAME}_SOURCES ${${PROJECT_NAME}_SOURCES};${OUT_SHADERS_LIST} CACHE INTERNAL "${PROJECT_NAME} sources" FORCE)
	endfunction()
else()
	function(add_shaders RES_DIR RES_GROUP INC_DIR INC_GROUP SHADERS_ROOT)
		add_module_sources(OUT_SHADERS_LIST
			SRC_GROUP ${RES_DIR} ${RES_GROUP}
				${ARGN}
		)
	endfunction()
endif()