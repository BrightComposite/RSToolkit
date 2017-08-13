#--------------------------------------------------------
#	ASD cmake module facilities
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

include(${ASD_TOOLS}/utils.cmake)
include(${ASD_TOOLS}/vendor.cmake)

#--------------------------------------------------------

if(WIN32)
	set(CMAKE_SHARED_LIBRARY_PREFIX "" CACHE INTERNAL "Cmake shared lib prefix")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(MODULE_ARCH x64 CACHE INTERNAL "Project architecture")
else()
	set(MODULE_ARCH x86 CACHE INTERNAL "Project architecture")
endif()

if("${MAKE_TARGET_DIR}" STREQUAL "")
	if("${CMAKE_GENERATOR}" STREQUAL "Visual Studio 14 2015")
		set(MAKE_TARGET_DIR build-vs-x86)
	elseif("${CMAKE_GENERATOR}" STREQUAL "Visual Studio 14 2015 Win64")
		set(MAKE_TARGET_DIR build-vs-x64)
	elseif("${CMAKE_GENERATOR}" STREQUAL "CodeBlocks - Unix Makefiles")
		set(MAKE_TARGET_DIR build-cb)
	elseif("${CMAKE_GENERATOR}" STREQUAL "Ninja")
		set(MAKE_TARGET_DIR build-ninja)
	elseif("${CMAKE_GENERATOR}" STREQUAL "Visual Studio 14 2015 Win64")
		set(MAKE_TARGET_DIR build-vs-x64)
	elseif("${CMAKE_GENERATOR}" STREQUAL "Unix Makefiles")
		set(MAKE_TARGET_DIR build-unix)
	else()
		set(MAKE_TARGET_DIR build)
	endif()
endif()

set(OUTPUT_PATH_SUFFIX "${MAKE_TARGET_DIR}" CACHE PATH "Output path suffix" FORCE)
set(OUTPUT_ROOT "${WORKSPACE_ROOT}/output/${OUTPUT_PATH_SUFFIX}" CACHE PATH "Output root" FORCE)

set(BINARY_OUTPUT ${WORKSPACE_ROOT}/output/bin/${MODULE_ARCH} CACHE PATH "Binary output" FORCE)
set(LIBRARY_OUTPUT ${WORKSPACE_ROOT}/output/lib/${MODULE_ARCH} CACHE PATH "Library output" FORCE)

set(MODULES_ROOT ${ASD_ROOT}/code/modules CACHE PATH "Modules root" FORCE)
set(WORKSPACE_MODULES_ROOT ${WORKSPACE_ROOT}/code/modules CACHE PATH "Workspace modules root" FORCE)

set(VENDOR_ROOT ${ASD_ROOT}/vendor CACHE PATH "Vendor directory" FORCE)
set(WORKSPACE_VENDOR_ROOT ${WORKSPACE_ROOT}/vendor CACHE PATH "Workspace vendor directory" FORCE)

if(NOT "${ALTERNATIVE_ROOT}" STREQUAL "")
	set(${PROJECT_NAME}_ROOT "${ALTERNATIVE_ROOT}" CACHE PATH "")
	unset(ALTERNATIVE_ROOT)
endif()

#--------------------------------------------------------

function(set_output_dir DIR TYPE)
	get_filename_component(REAL_DIR ${DIR} REALPATH)
	set(OUTPUT_DIR CMAKE_${TYPE}_OUTPUT_DIRECTORY)

	set(${OUTPUT_DIR} ${REAL_DIR}/${MODULE_ARCH} CACHE INTERNAL "Cmake ${TYPE} output")

	set(${OUTPUT_DIR}_DEBUG ${${OUTPUT_DIR}}/debug CACHE PATH "Cmake ${TYPE} debug output")
	set(${OUTPUT_DIR}_RELEASE ${${OUTPUT_DIR}}/release CACHE PATH "Cmake ${TYPE} release output")
	set(${OUTPUT_DIR}_MINSIZEREL ${${OUTPUT_DIR}}/minsize CACHE PATH "Cmake ${TYPE} minsizerelease output")
	set(${OUTPUT_DIR}_RELWITHDEBINFO ${${OUTPUT_DIR}}/withdebinfo CACHE PATH "Cmake ${TYPE} relwithdebinfo output")
endfunction()

set_output_dir(${WORKSPACE_ROOT}/output/lib ARCHIVE)
set_output_dir(${WORKSPACE_ROOT}/output/lib LIBRARY)
set_output_dir(${WORKSPACE_ROOT}/output/bin RUNTIME)

link_directories(${WORKSPACE_ROOT}/output/lib/${MODULE_ARCH})
link_directories(${WORKSPACE_ROOT}/output/lib/${MODULE_ARCH}/debug)
link_directories(${WORKSPACE_ROOT}/output/lib/${MODULE_ARCH}/release)

#--------------------------------------------------------

set(MODULE_TYPES APPLICATION;SHARED;LIBRARY;STATIC;INLINE CACHE INTERNAL "Module types" FORCE)

#--------------------------------------------------------

if(NOT ";${GUARD_BLOCKS};" MATCHES ";MODULE_TOOL_GUARD;")
	set(GUARD_BLOCKS ${GUARD_BLOCKS};MODULE_TOOL_GUARD CACHE INTERNAL "Guard blocks" FORCE)

#--------------------------------------------------------
#	Functions
#--------------------------------------------------------

#	group function

	function(group path name)
		if("${path}" STREQUAL "")
			message(FATAL_ERROR "The group path may not be empty!")
		endif()

		string(REPLACE / \\ path ${path})
		set(CURRENT_SOURCE_GROUP_PATH ${path} CACHE STRING "" FORCE)
		set(CURRENT_SOURCE_GROUP_NAME ${name} CACHE STRING "" FORCE)

		if(NOT ";${${PROJECT_NAME}_SOURCE_GROUPS};" MATCHES ";${path};")
			set(${PROJECT_NAME}_SOURCE_GROUPS ${${PROJECT_NAME}_SOURCE_GROUPS};${path} CACHE INTERNAL "" FORCE)
		endif()

	endfunction()

#	domain function

	function(domain path)
		string(REPLACE \\ / path ${path})
		set(CURRENT_SOURCE_DOMAIN ${path} CACHE STRING "" FORCE)
	endfunction()

#	set_source function

	function(set_source out dir source)
		string(REGEX REPLACE "/$" "" dir "${dir}")
		if("${dir}" STREQUAL "")
			set(${out} ${source} PARENT_SCOPE)
		else()
			set(${out} ${dir}/${source} PARENT_SCOPE)
		endif()
	endfunction()

#	flush_sources function

	function(flush_sources dir)
		if(NOT "${ARGN}" STREQUAL "")
			string(REGEX REPLACE "/$" "" dir "${dir}")
			string(REPLACE / \\ dir "${dir}")

			if("${dir}" STREQUAL "")
				source_group(${CURRENT_SOURCE_GROUP_NAME} FILES ${ARGN})
			else()
				source_group(${CURRENT_SOURCE_GROUP_NAME}\\${dir} FILES ${ARGN})
			endif()

			set(CURRENT_SOURCE_LIST ${CURRENT_SOURCE_LIST} ${ARGN} CACHE INTERNAL "" FORCE)
		endif()
	endfunction()

#	collect_files function.
#	Usage:
#		collect_files(<output list> <tree...>)

	function(collect_files out)
		set(source_list)
		set(dir)
		set(CURRENT_SOURCE_LIST CACHE INTERNAL "" FORCE)
		set(root ${PROJECT_SOURCE_DIR}/${CURRENT_SOURCE_GROUP_PATH})

		foreach(entry ${ARGN})
			if("${entry}" MATCHES ".*/$" OR "${entry}" MATCHES ".*\\$")
				flush_sources(${CURRENT_SOURCE_DOMAIN}/${dir} ${dir_list})
				set(dir_list)

				string(REPLACE "\\" "/" entry ${entry})
				string(REGEX REPLACE "/$" "" entry ${entry})

				if("${dir}" STREQUAL "")
					set(dir ${entry})
				else()
					set(dir ${dir}/${entry})
				endif()
				string(REPLACE "//" "/" dir ${dir})
			elseif("${entry}" STREQUAL "..")
				flush_sources(${CURRENT_SOURCE_DOMAIN}/${dir} ${dir_list})
				set(dir_list)
				get_filename_component(dir ${dir} DIRECTORY)

				if("${dir}" STREQUAL "/")
					set(dir)
				endif()
			elseif("${entry}" STREQUAL ".")
				flush_sources(${CURRENT_SOURCE_DOMAIN}/${dir} ${dir_list})
				set(dir_list)
				set(dir)
			else()
				set_source(source ${CURRENT_SOURCE_DOMAIN}/${dir} ${entry})

				if(NOT EXISTS "${root}/${source}")
					create_source(${root} ${source})
				endif()

				list(APPEND dir_list ${root}/${source})
			endif()
		endforeach()

		flush_sources(${CURRENT_SOURCE_DOMAIN}/${dir} ${dir_list})
		set(${out} ${CURRENT_SOURCE_LIST} PARENT_SCOPE)
		set(${PROJECT_NAME}_SOURCES ${${PROJECT_NAME}_SOURCES};${CURRENT_SOURCE_LIST} CACHE INTERNAL "${PROJECT_NAME} sources" FORCE)
	endfunction()

#	files function.
#	Usage:
#		files(<tree...>)

	function(files)
		collect_files(_ ${ARGN})
	endfunction()

#	add_module_include_dirs function.

	function(add_module_include_dirs)
		if("${${PROJECT_NAME}_MODULE_TYPE}" STREQUAL "INLINE")
			target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${ARGN})
		else()
			target_include_directories(${PROJECT_NAME} PRIVATE ${ARGN})
		endif()

		set(DIRS ${${PROJECT_NAME}_INCLUDE_DIRS};${ARGN})
		list(REMOVE_DUPLICATES DIRS)
		set(${PROJECT_NAME}_INCLUDE_DIRS ${DIRS} CACHE INTERNAL "${PROJECT_NAME} include directories" FORCE)
	endfunction()

#	add_module_libraries function.

	function(add_module_libraries)
		if("${${PROJECT_NAME}_MODULE_TYPE}" STREQUAL "INLINE")
			foreach(LIB ${ARGN})
				target_link_libraries(${PROJECT_NAME} INTERFACE ${LIB})
			endforeach()
		else()
			foreach(LIB ${ARGN})
				target_link_libraries(${PROJECT_NAME} PUBLIC ${LIB})
			endforeach()
		endif()
	endfunction()

#	add_module function.

	function(add_module PATH)
		if("${PATH}" STREQUAL "")
			message(FATAL_ERROR "Module path must be non-empty!")
		endif()

		set(DIR "${PATH}")
		set(DOMAIN "${ARGV1}")
		set(ROOT "${ARGV2}")

		if("${DOMAIN}" STREQUAL "")
			set(DOMAIN modules)
		endif()

		set(DIR "${DOMAIN}/${DIR}")

		if(NOT "${ROOT}" STREQUAL "")
			if(NOT IS_ABSOLUTE "${ROOT}")
				message(FATAL_ERROR "add_module: a third argument (module root) must be an absolute path or empty")
			endif()

			set(OUTPUT_DIR "${ROOT}/${OUTPUT_PATH_SUFFIX}/${DIR}")
			set(DIR "${ROOT}/code/${DIR}")
		else()
			if("${DOMAIN}" STREQUAL "modules")
				set(OUTPUT_DIR "${OUTPUT_ROOT}/${PATH}")
			else()
				set(OUTPUT_DIR "${OUTPUT_ROOT}/${DIR}")
			endif()

			set(DIR "${WORKSPACE_ROOT}/code/${DIR}")
		endif()

		if(NOT EXISTS "${DIR}/CMakeLists.txt")
			file(MAKE_DIRECTORY ${DIR})

			set(TEMPLATE_FILE ${ASD_ROOT}/templates/module/CMakeLists.txt)
			set(TEMPLATE_FILE_DOMAIN ${ASD_ROOT}/templates/module/${DOMAIN}/CMakeLists.txt)

			set(module_path ${PATH})
			set(module_domain ${DOMAIN})

			if(EXISTS "${TEMPLATE_FILE_DOMAIN}")
				configure_file(${TEMPLATE_FILE_DOMAIN} ${DIR}/CMakeLists.txt @ONLY)
			elseif(EXISTS "${TEMPLATE_FILE}")
				configure_file(${TEMPLATE_FILE} ${DIR}/CMakeLists.txt @ONLY)
			else()
				message(FATAL_ERROR "Can't create new module \"${PATH}\"")
			endif()
		endif()

		if(NOT "${ROOT}" STREQUAL "")
			set(ALTERNATIVE_ROOT "${ROOT}" CACHE PATH "")
		endif()

		add_subdirectory("${DIR}" "${OUTPUT_DIR}")
	endfunction()

	function(add_test PATH)
		add_module(${PATH} tests ${ARGV1})
	endfunction()
	
	function(add_command command)
		set(DIR "${command}")
		set(ROOT "${ARGV1}")
		set(DOMAIN "commands")

		set(DIR "${DOMAIN}/${DIR}")

		if(NOT "${ROOT}" STREQUAL "")
			if(NOT IS_ABSOLUTE "${ROOT}")
				message(FATAL_ERROR "add_command: a third argument (module root) must be an absolute path or empty")
			endif()

			set(OUTPUT_DIR "${ROOT}/output/${OUTPUT_PATH_SUFFIX}/${DIR}")
			set(DIR "${ROOT}/code/${DIR}")
		else()
			set(OUTPUT_DIR "${OUTPUT_ROOT}/${DIR}")
			set(DIR "${WORKSPACE_ROOT}/code/${DIR}")
		endif()

		if(NOT EXISTS "${DIR}/CMakeLists.txt")
			file(MAKE_DIRECTORY ${DIR})

			set(TEMPLATE_FILE ${ASD_ROOT}/templates/command/CMakeLists.txt)

			set(module_path ${PATH})
			set(module_domain ${DOMAIN})

			if(EXISTS "${TEMPLATE_FILE}")
				configure_file(${TEMPLATE_FILE} ${DIR}/CMakeLists.txt @ONLY)
			else()
				message(FATAL_ERROR "Can't create new module \"${PATH}\"")
			endif()
		endif()

		if(NOT "${ROOT}" STREQUAL "")
			set(ALTERNATIVE_ROOT "${ROOT}" CACHE PATH "")
		endif()

		add_subdirectory("${DIR}" "${OUTPUT_DIR}")
	endfunction()

#	require_module function.

	function(require_module module_id version)
		set(MESSAGES_INDENTATION_TEMP ${MESSAGES_INDENTATION})
		set(MESSAGES_INDENTATION "${MESSAGES_INDENTATION}  " CACHE STRING "Messages indentation" FORCE)

		message("${MESSAGES_INDENTATION}> Depends on \"${module_id}-${version}\"")

		if(NOT ";${GUARD_BLOCKS};" MATCHES ";${module_id}_GUARD;")
			set(module_path)
			color_message(BLACK "${MESSAGES_INDENTATION}* Try to resolve...")

			if(NOT "${${PROJECT_NAME}_ROOT}" STREQUAL "" AND EXISTS "${${PROJECT_NAME}_ROOT}/code/modules/${module_id}")
				set(module_path "${${PROJECT_NAME}_ROOT}/code/modules/${module_id}")
				color_message(WHITE "${MESSAGES_INDENTATION}* Found at ${module_path}")
				add_subdirectory(${module_path} "${${PROJECT_NAME}_ROOT}/output/${OUTPUT_PATH_SUFFIX}/${module_id}")

			elseif(EXISTS "${WORKSPACE_MODULES_ROOT}/${module_id}")
				set(module_path "${WORKSPACE_MODULES_ROOT}/${module_id}")
				color_message(WHITE "${MESSAGES_INDENTATION}* Found at ${module_path}")
				add_subdirectory(${module_path} "${OUTPUT_ROOT}/${module_id}")

			elseif(EXISTS "${MODULES_ROOT}/${module_id}")
				set(module_path "${MODULES_ROOT}/${module_id}")
				color_message(WHITE "${MESSAGES_INDENTATION}* Found at ${module_path}")
				add_subdirectory(${module_path} "${ASD_ROOT}/output/${OUTPUT_PATH_SUFFIX}/${module_id}")

			elseif(EXISTS "${WORKSPACE_VENDOR_ROOT}/${module_id}")
				set(module_path "${WORKSPACE_VENDOR_ROOT}/${module_id}")
				color_message(WHITE "${MESSAGES_INDENTATION}* Found at ${module_path}")
				add_subdirectory(${module_path} "${OUTPUT_ROOT}/${module_id}")

			elseif(EXISTS "${VENDOR_ROOT}/${module_id}")
				set(module_path "${VENDOR_ROOT}/${module_id}")
				color_message(WHITE "${MESSAGES_INDENTATION}* Found at ${module_path}")
				add_subdirectory(${module_path} "${ASD_ROOT}/output/${OUTPUT_PATH_SUFFIX}/${module_id}")

			else()
				message(FATAL_ERROR "Couldn't resolve dependency \"${module_id}-${version}\"!")
			endif()

			if(NOT ";${GUARD_BLOCKS};" MATCHES ";${module_id}_GUARD;")
				message(FATAL_ERROR "${module_path} is not a module path!")
			endif()
		endif()

		set(name ${PROJECT_NAME_OF_${module_id}})
		set(vers ${PROJECT_VERSION_OF_${module_id}})
		color_message(BLACK "${MESSAGES_INDENTATION}* Use \"${name} (${vers})\"")

		set(${PROJECT_NAME}_MODULE_DEPENDENCIES ${${PROJECT_NAME}_MODULE_DEPENDENCIES};${name} CACHE INTERNAL "${PROJECT_NAME} module dependencies" FORCE)

		set(MESSAGES_INDENTATION ${MESSAGES_INDENTATION_TEMP} CACHE STRING "Messages indentation" FORCE)
	endfunction()

#	dependencies function.

	function(dependencies)
		set(MODULE_ID)

		foreach(ENTRY ${ARGN})
			if("${MODULE_ID}" STREQUAL "")
				set(MODULE_ID ${ENTRY})
			else()
				require_module(${MODULE_ID} ${ENTRY})
				set(MODULE_ID)
			endif()
		endforeach()
	endfunction()

#	module function.

	function(module type)
		get_filename_component(PROJECT_ID ${PROJECT_SOURCE_DIR} NAME)
		get_filename_component(PROJECT_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)
		get_filename_component(PROJECT_DIR "${PROJECT_DIR}" NAME)
		
		if(NOT "${PROJECT_DIR}" STREQUAL "modules")
			set(PROJECT_ID ${PROJECT_ID}_${PROJECT_DIR})
			message("${PROJECT_ID}")
		endif()
		
		set(PROJECT_NAME_OF_${PROJECT_ID} ${PROJECT_NAME} CACHE STRING "project name of ${PROJECT_ID} module")
		set(PROJECT_VERSION_OF_${PROJECT_ID} ${PROJECT_VERSION} CACHE STRING "project version of ${PROJECT_ID} module")

		if(NOT ";${GUARD_BLOCKS};" MATCHES ";${PROJECT_ID}_GUARD;")
			set(GUARD_BLOCKS ${GUARD_BLOCKS};${PROJECT_ID}_GUARD CACHE INTERNAL "Guard blocks" FORCE)
		endif()

		message("")
		
		if("${type}" STREQUAL "APPLICATION")
			if("${ARGN}" STREQUAL "CONSOLE")
				color_message(GREEN "${MESSAGES_INDENTATION}+ Add console app \"${PROJECT_NAME}\"")
				add_definitions("-DASD_CONSOLE")
			else()
				color_message(GREEN "${MESSAGES_INDENTATION}+ Add app \"${PROJECT_NAME}\"")
			endif()
		else()
			color_message(GREEN "${MESSAGES_INDENTATION}+ Add module \"${PROJECT_NAME}\"")
		endif()

		set(${PROJECT_NAME}_SOURCES CACHE INTERNAL "${PROJECT_NAME} sources" FORCE)
		set(${PROJECT_NAME}_INCLUDE_DIRS CACHE INTERNAL "${PROJECT_NAME} include directories" FORCE)
		set(${PROJECT_NAME}_MODULE_DEPENDENCIES CACHE INTERNAL "${PROJECT_NAME} module dependencies" FORCE)

		if("${type}" STREQUAL "")
			message(FATAL_ERROR "Incorrect usage of 'module' function! Type of module is not set. List of correct types: (${MODULE_TYPES})")
		elseif(NOT ";${MODULE_TYPES};" MATCHES ";${type};")
			message(FATAL_ERROR "Incorrect usage of 'module' function! List of correct types: (${MODULE_TYPES}). Provided type is ${type}")
		endif()

		set(${PROJECT_NAME}_MODULE_TYPE ${type} CACHE STRING "${PROJECT_NAME} module type" FORCE)
	endfunction()

#	collect_dependencies function.

	function(collect_dependencies DEPENDENCIES MODULE)
		if(NOT "${${MODULE}_MODULE_DEPENDENCIES}" STREQUAL "")
			set(DEPS)

			foreach(DEPENDENCY ${${MODULE}_MODULE_DEPENDENCIES})
				collect_dependencies(MODULE_DEPS ${DEPENDENCY})
				list(APPEND DEPS ${MODULE_DEPS})
			endforeach()

			list(APPEND DEPS ${${MODULE}_MODULE_DEPENDENCIES})

			if(NOT "${DEPS}" STREQUAL "")
				list(REMOVE_DUPLICATES DEPS)
				set(${DEPENDENCIES} ${DEPS} PARENT_SCOPE)
			endif()
		endif()
	endfunction()

#	sources function

	function(sources folder)
		set(${PROJECT_NAME}_FOLDER ${folder} CACHE STRING "${PROJECT_NAME} folder" FORCE)

		set(CURRENT_SOURCE_DOMAIN CACHE STRING "" FORCE)
		set(CURRENT_SOURCE_GROUP_PATH src CACHE STRING "" FORCE)
		set(CURRENT_SOURCE_GROUP_NAME Sources CACHE STRING "" FORCE)
	endfunction()

#	endsources function

	function(endsources)
		set(CURRENT_SOURCE_DOMAIN CACHE STRING "" FORCE)
		set(CURRENT_SOURCE_GROUP_PATH src CACHE STRING "" FORCE)
		set(CURRENT_SOURCE_GROUP_NAME Sources CACHE STRING "" FORCE)
	endfunction()

#	api function.

	function(api key)
		set(${PROJECT_NAME}_API_KEY ${key} CACHE STRING "${PROJECT_NAME} api key" FORCE)
	endfunction()

#	api_export function.

	function(api_export MODULE)
		if(NOT "${${MODULE}_API_KEY}" STREQUAL "")
			set(module ${${MODULE}_API_KEY})
		else()
			name_lower(module ${MODULE})
		endif()

		if(WIN32 AND "${${MODULE}_MODULE_TYPE}" STREQUAL "SHARED")
			add_definitions("-Dasd_${module}_api=__declspec(dllexport)")
		else()
			add_definitions("-Dasd_${module}_api=")
		endif()
	endfunction()

#	api_import function.

	function(api_import MODULE)
		if(NOT "${${MODULE}_API_KEY}" STREQUAL "")
			set(module ${${MODULE}_API_KEY})
		else()
			name_lower(module ${MODULE})
		endif()

		if(WIN32 AND "${${MODULE}_MODULE_TYPE}" STREQUAL "SHARED")
			add_definitions("-Dasd_${module}_api=__declspec(dllimport)")
		else()
			add_definitions("-Dasd_${module}_api=")
		endif()
	endfunction()

#	endmodule function.

	function(endmodule)
		if("${${PROJECT_NAME}_MODULE_TYPE}" STREQUAL "APPLICATION")
			add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})
		elseif("${${PROJECT_NAME}_MODULE_TYPE}" STREQUAL "SHARED")
			add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCES})
		elseif("${${PROJECT_NAME}_MODULE_TYPE}" STREQUAL "LIBRARY" OR "${${PROJECT_NAME}_MODULE_TYPE}" STREQUAL "STATIC")
			add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})
		elseif("${${PROJECT_NAME}_MODULE_TYPE}" STREQUAL "INLINE")
			add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})
		elseif("${${PROJECT_NAME}_MODULE_TYPE}" STREQUAL "")
			message(FATAL_ERROR "Module type is not set! Call 'module' first!")
		else()
			message(FATAL_ERROR "Module type is incorrect! Check the 'module' function call")
		endif()

		set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
		#set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

		if(NOT "${${PROJECT_NAME}_FOLDER}" STREQUAL "")
			set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${${PROJECT_NAME}_FOLDER})
		endif()

		target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS})

		foreach(GROUP ${${PROJECT_NAME}_SOURCE_GROUPS})
			add_module_include_dirs(${PROJECT_SOURCE_DIR}/${GROUP})
		endforeach()

		api_export(${PROJECT_NAME})

		collect_dependencies(DEPENDENCIES ${PROJECT_NAME})
		message("${DEPENDENCIES}")

		if("${${PROJECT_NAME}_MODULE_TYPE}" STREQUAL "INLINE")
			foreach(DEPENDENCY ${DEPENDENCIES})
				target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${${DEPENDENCY}_INCLUDE_DIRS})
			endforeach()
		else()
			foreach(DEPENDENCY ${DEPENDENCIES})
				target_include_directories(${PROJECT_NAME} PRIVATE ${${DEPENDENCY}_INCLUDE_DIRS})

				if(NOT "${${DEPENDENCY}_MODULE_TYPE}" STREQUAL "INLINE")
					target_link_libraries(${PROJECT_NAME} PRIVATE ${DEPENDENCY})
				endif()
			endforeach()
		endif()

		foreach(DEPENDENCY ${DEPENDENCIES})
			api_import(${DEPENDENCY})
		endforeach()
	endfunction()
endif()
