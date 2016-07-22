#--------------------------------------------------------
#	Rapture State cmake project facilities
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.0)

#--------------------------------------------------------

if(WIN32)
	set(CMAKE_SHARED_LIBRARY_PREFIX "" CACHE INTERNAL "Cmake shared lib prefix")
endif ()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(MODULE_ARCH x64 CACHE INTERNAL "Project architecture")
else()
	set(MODULE_ARCH x86 CACHE INTERNAL "Project architecture")
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(OUTPUT_ROOT ${RAPTURE_ROOT}/build-vs-${MODULE_ARCH} CACHE PATH "Output root")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
	set(OUTPUT_ROOT ${RAPTURE_ROOT}/build-cb CACHE PATH "Output root")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
	set(OUTPUT_ROOT ${RAPTURE_ROOT}/build-clang CACHE PATH "Output root")
endif()

set(MODULES_ROOT ${RAPTURE_ROOT}/modules CACHE PATH "Modules root")

set(BINARY_OUTPUT ${RAPTURE_ROOT}/bin/${MODULE_ARCH} CACHE PATH "Binary output")
set(LIBRARY_OUTPUT ${RAPTURE_ROOT}/bin/${MODULE_ARCH} CACHE PATH "Library output")

set(THIRD_PARTY ${RAPTURE_ROOT}/third-party CACHE PATH "Third-party directory")

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

set_output_dir(${RAPTURE_ROOT}/lib ARCHIVE)
set_output_dir(${RAPTURE_ROOT}/lib LIBRARY)
set_output_dir(${RAPTURE_ROOT}/bin RUNTIME)

link_directories(${RAPTURE_ROOT}/lib/${MODULE_ARCH})
link_directories(${RAPTURE_ROOT}/lib/${MODULE_ARCH}/release)
link_directories(${RAPTURE_ROOT}/lib/${MODULE_ARCH}/debug)

#--------------------------------------------------------

set(MODULE_TYPES APPLICATION;SHARED;LIBRARY;INLINE CACHE INTERNAL "Module types" FORCE)

#--------------------------------------------------------

if(NOT ";${GUARD_BLOCKS};" MATCHES ";MODULE_TOOL_GUARD;")
	set(GUARD_BLOCKS ${GUARD_BLOCKS};MODULE_TOOL_GUARD CACHE INTERNAL "Guard blocks" FORCE)

#--------------------------------------------------------
#	Functions
#--------------------------------------------------------

	function(name_upper out name)
		string(REGEX REPLACE "([a-z])([A-Z])" "\\1_\\2" NAME ${name})
		string(REGEX REPLACE "[^A-Za-z0-9_]" "_" NAME ${NAME})
		string(TOUPPER ${NAME} NAME)

		set(${out} ${NAME} PARENT_SCOPE)
	endfunction()

	function(name_lower out name)
		string(REGEX REPLACE "([a-z])([A-Z])" "\\1_\\2" NAME ${name})
		string(REGEX REPLACE "[^A-Za-z0-9_]" "_" NAME ${NAME})
		string(TOLOWER ${NAME} NAME)

		set(${out} ${NAME} PARENT_SCOPE)
	endfunction()

#	create_source function.

	function(create_source SRC_ROOT SRC_PATH)
		set(SRC_FULLPATH ${SRC_ROOT}/${SRC_PATH})

		get_filename_component(SRC_DIR  ${SRC_FULLPATH} DIRECTORY)
		get_filename_component(SRC_EXT  ${SRC_FULLPATH} EXT)
		get_filename_component(SRC_SHORT_DIR  ${SRC_PATH} DIRECTORY)

		get_filename_component(filename ${SRC_FULLPATH} NAME_WE)
		set(short_path ${SRC_SHORT_DIR}/${filename})

		if(NOT EXISTS ${SRC_DIR})
			file(MAKE_DIRECTORY ${SRC_DIR})
		endif()

		name_upper(FILE_NAME ${filename})
		name_upper(PROJECT_NAME_UPPER ${PROJECT_NAME})
		name_lower(PROJECT_NAME_LOWER ${PROJECT_NAME})

		set(TEMPLATE_FILE ${RAPTURE_ROOT}/templates/${filename}${SRC_EXT})
		set(TEMPLATE_FILE_EXT ${RAPTURE_ROOT}/templates/_${SRC_EXT})

		message(STATUS "Create new source file ${SRC_FULLPATH}...")

		if(EXISTS ${TEMPLATE_FILE})
			configure_file(${TEMPLATE_FILE} ${SRC_FULLPATH})
		elseif(EXISTS ${TEMPLATE_FILE_EXT})
			configure_file(${TEMPLATE_FILE_EXT} ${SRC_FULLPATH})
		else()
			file(WRITE ${SRC_FULLPATH} "")
		endif()
	endfunction()

	function(group path name)
		string(REPLACE / \\ path ${path})
		set(CURRENT_SOURCE_GROUP_PATH ${path} CACHE INTERNAL "")
		set(CURRENT_SOURCE_GROUP_NAME ${name} CACHE INTERNAL "")
	endfunction()
	
#	collect_files function.
#	Usage:
#		collect_files(<output list> <tree...>)

	function(collect_files out)
		set(source_list)

		set(dir "")
		set(root ${PROJECT_SOURCE_DIR}/${CURRENT_SOURCE_GROUP_PATH})

		foreach(entry ${ARGN})
			if(${entry} MATCHES ".*/" OR ${entry} MATCHES ".*\\")
				string(REGEX REPLACE "[/\\]$" "" entry ${entry})
				string(REPLACE "\\" "/" entry ${entry})
				set(dir ${dir}/${entry})
			else()
				if(NOT ${dir} STREQUAL "")
					set(source ${dir}/${entry})
				else()
					set(source ${entry})
				endif()

				if(NOT EXISTS ${root}/${source})
					create_source(${root} ${source})
				endif()

				list(APPEND source_list ${root}/${source})
			endif()
		endforeach()

		list(LENGTH source_list source_count)

		if(NOT ${source_count} EQUAL 0)
			source_group(${CURRENT_SOURCE_GROUP_NAME} FILES ${source_list})
		endif()

		set(${out} ${source_list} PARENT_SCOPE)
	endfunction()

#	files function.
#	Usage:
#		files(<tree...>)

	function(files)
		collect_files(OUT_LIST ${ARGN})
		set(${PROJECT_NAME}_SOURCES ${${PROJECT_NAME}_SOURCES};${OUT_LIST} CACHE INTERNAL "${PROJECT_NAME} sources" FORCE)
	endfunction()

#	add_module_include_dirs function.

	function(add_module_include_dirs)
		target_include_directories(${PROJECT_NAME} PUBLIC ${ARGN})

		set(DIRS ${${PROJECT_NAME}_INCLUDE_DIRS};${ARGN})
		list(REMOVE_DUPLICATES DIRS)
		set(${PROJECT_NAME}_INCLUDE_DIRS ${DIRS} CACHE INTERNAL "${PROJECT_NAME} include directories" FORCE)
	endfunction()

#	add_module_libraries function.

	function(add_module_libraries)
		foreach(LIB ${ARGN})
			if(NOT "${${LIB}_MODULE_TYPE}" STREQUAL "INLINE")
				target_link_libraries(${PROJECT_NAME} PUBLIC ${LIB})
			else()
				add_module_include_dirs(${${LIB}_INCLUDE_DIRS})
			endif()
		endforeach()
	endfunction()

#	add_module function.

	function(add_module path root)
		add_subdirectory("${root}${path}" "${OUTPUT_ROOT}/${path}")
	endfunction()

#	require_module function.

	function(require_module name path)
		set(MESSAGES_INDENTATION_TEMP ${MESSAGES_INDENTATION})
		set(MESSAGES_INDENTATION "${MESSAGES_INDENTATION}  " CACHE STRING "Dependency list indentation" FORCE)
		message("${MESSAGES_INDENTATION}> Depends on \"${name}\"")

		set(${PROJECT_NAME}_MODULE_DEPENDENCIES ${${PROJECT_NAME}_MODULE_DEPENDENCIES};${name} CACHE INTERNAL "${PROJECT_NAME} module dependencies" FORCE)

		if(NOT ";${GUARD_BLOCKS};" MATCHES ";${name}_GUARD;")
			add_subdirectory("${MODULES_ROOT}/${path}" "${OUTPUT_ROOT}/${path}")
		endif()

		set(MESSAGES_INDENTATION ${MESSAGES_INDENTATION_TEMP} CACHE STRING "Dependency list indentation" FORCE)
	endfunction()

#	dependencies function.

	function(dependencies)
		set(MODULE_NAME)

		foreach(ENTRY ${ARGN})
			if("${MODULE_NAME}" STREQUAL "")
				set(MODULE_NAME ${ENTRY})
			else()
				require_module(${MODULE_NAME} ${ENTRY})
				set(MODULE_NAME)
			endif()
		endforeach()
	endfunction()

#	module function.

	function(module type)
		if(NOT ";${GUARD_BLOCKS};" MATCHES ";${PROJECT_NAME}_GUARD;")
			set(GUARD_BLOCKS ${GUARD_BLOCKS};${PROJECT_NAME}_GUARD CACHE INTERNAL "Guard blocks" FORCE)
		endif()

		message("${MESSAGES_INDENTATION}+ Add module \"${PROJECT_NAME}\"")

		set(${PROJECT_NAME}_SOURCES CACHE INTERNAL "${PROJECT_NAME} sources" FORCE)
		set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include CACHE INTERNAL "${PROJECT_NAME} include directories" FORCE)
		set(${PROJECT_NAME}_MODULE_DEPENDENCIES CACHE INTERNAL "${PROJECT_NAME} module dependencies" FORCE)

		if("${type}" STREQUAL "")
			message(FATAL_ERROR "Incorrect usage of 'start_module' function! Type of module is not set. List of correct types: (${MODULE_TYPES})")
		elseif(NOT ";${MODULE_TYPES};" MATCHES ";${type};")
			message(FATAL_ERROR "Incorrect usage of 'start_module' function! List of correct types: (${MODULE_TYPES}). Provided type is ${type}")
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
	endfunction()

#	api function.

	function(api key)
		set(${PROJECT_NAME}_API_KEY ${key} CACHE STRING "${PROJECT_NAME} api key" FORCE)
	endfunction()

#	api_export function.

	function(api_export MODULE)
		if(NOT "${${MODULE}_API_KEY}" STREQUAL "")
			set(module rapture_${${MODULE}_API_KEY})
		else()
			name_lower(module ${MODULE})
		endif()

		if(WIN32)
			add_definitions("-D${module}_api=__declspec(dllexport)")
		else()
			add_definitions("-D${module}_api")
		endif()
	endfunction()

#	api_import function.

	function(api_import MODULE)
		if(NOT "${${MODULE}_API_KEY}" STREQUAL "")
			set(module rapture_${${MODULE}_API_KEY})
		else()
			name_lower(module ${MODULE})
		endif()

		if(WIN32)
			add_definitions("-D${module}_api=__declspec(dllimport)")
		else()
			add_definitions("-D${module}_api")
		endif()
	endfunction()

#	endmodule function.

	function(endmodule)
		if("${${PROJECT_NAME}_MODULE_TYPE}" STREQUAL "APPLICATION")
			add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})
		elseif("${${PROJECT_NAME}_MODULE_TYPE}" STREQUAL "SHARED")
			add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCES})
		elseif("${${PROJECT_NAME}_MODULE_TYPE}" STREQUAL "LIBRARY")
			add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})
		elseif("${${PROJECT_NAME}_MODULE_TYPE}" STREQUAL "INLINE")
			add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})
		elseif("${${PROJECT_NAME}_MODULE_TYPE}" STREQUAL "")
			message(FATAL_ERROR "Module type is not set! Call 'start_module' first!")
		else()
			message(FATAL_ERROR "Module type is incorrect! Check the 'start_module' function call")
		endif()

		set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
		set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

		if(NOT "${${PROJECT_NAME}_FOLDER}" STREQUAL "")
			set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${${PROJECT_NAME}_FOLDER})
		endif()

		target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS})

		add_module_include_dirs(${PROJECT_SOURCE_DIR}/include)
		add_module_libraries(${${PROJECT_NAME}_MODULE_DEPENDENCIES})
		api_export(${PROJECT_NAME})

		collect_dependencies(DEPENDENCIES ${PROJECT_NAME})

		foreach(DEPENDENCY_NAME ${DEPENDENCIES})
			if("${${DEPENDENCY_NAME}_MODULE_TYPE}" STREQUAL "SHARED")
				api_import(${DEPENDENCY_NAME})
			endif()
		endforeach()
	endfunction()

#	setup_file function

	function(setup_file FILE_PATH SOURCE DEST)
		if(NOT EXISTS ${DEST}/${FILE_PATH})
			get_filename_component(FILE_DIR ${FILE_PATH} DIRECTORY)
			file(COPY ${SOURCE}/${FILE_PATH} DESTINATION ${DEST}/${FILE_DIR})
		endif()
	endfunction()

#	setup_third_party function

	function(setup_third_party LIBRARY_PATH)
		set(THIRD_PARTY_DIR ${THIRD_PARTY}/${LIBRARY_PATH})

		if(NOT EXISTS ${THIRD_PARTY_DIR})
			if(NOT EXISTS "${THIRD_PARTY_DIR}.tar.gz")
				message(FATAL_ERROR "Can't find the archive with third-party library '${LIBRARY_PATH}'!")
			endif()

			message(STATUS "Extract third-party library '${LIBRARY_PATH}' from ${THIRD_PARTY_DIR}.tar.gz")
			execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz "${LIBRARY_PATH}.tar.gz" WORKING_DIRECTORY "${THIRD_PARTY}")

			if(NOT EXISTS ${THIRD_PARTY_DIR})
				message(FATAL_ERROR "Can't extract the archive with third-party library '${LIBRARY_PATH}'!")
			endif()
		endif()

		if("${ARGV1}" STREQUAL "COMPONENT" AND NOT "${ARGV2}" STREQUAL "")
			message("${MESSAGES_INDENTATION}  Depends on ${LIBRARY_PATH}:${ARGV2}")
			set(PATH_SUFFIX ${ARGV2}/${MODULE_ARCH})
		else()
			message("${MESSAGES_INDENTATION}  Depends on ${LIBRARY_PATH}")
			set(PATH_SUFFIX ${MODULE_ARCH})
		endif()

		set(BINARY_SOURCE ${THIRD_PARTY_DIR}/bin/${PATH_SUFFIX})
		set(LIBRARY_SOURCE ${THIRD_PARTY_DIR}/lib/${PATH_SUFFIX})

		set(BINARY_DEST ${RAPTURE_ROOT}/bin/${MODULE_ARCH})
		set(LIBRARY_DEST ${RAPTURE_ROOT}/lib/${MODULE_ARCH})

		if(EXISTS ${BINARY_SOURCE})
			file(GLOB_RECURSE SHARED_LIBS RELATIVE ${BINARY_SOURCE}
				${BINARY_SOURCE}/*${CMAKE_SHARED_LIBRARY_SUFFIX}
			)

			foreach(SHARED_LIB ${SHARED_LIBS})
				get_filename_component(SHARED_LIB_NAME ${SHARED_LIB} NAME)
				get_filename_component(SHARED_LIB_DIR ${SHARED_LIB} DIRECTORY)

				setup_file(${SHARED_LIB_NAME} ${BINARY_SOURCE}/${SHARED_LIB_DIR} ${BINARY_DEST}/${SHARED_LIB_DIR}/release)
				setup_file(${SHARED_LIB_NAME} ${BINARY_SOURCE}/${SHARED_LIB_DIR} ${BINARY_DEST}/${SHARED_LIB_DIR}/minsize)
				setup_file(${SHARED_LIB_NAME} ${BINARY_SOURCE}/${SHARED_LIB_DIR} ${BINARY_DEST}/${SHARED_LIB_DIR}/withdebinfo)
				setup_file(${SHARED_LIB_NAME} ${BINARY_SOURCE}/${SHARED_LIB_DIR} ${BINARY_DEST}/${SHARED_LIB_DIR}/debug)
			endforeach()
		endif()

		if(EXISTS ${LIBRARY_SOURCE})
			file(GLOB_RECURSE STATIC_LIBS RELATIVE ${LIBRARY_SOURCE}
				${LIBRARY_SOURCE}/*${CMAKE_STATIC_LIBRARY_SUFFIX}
			)

			foreach(STATIC_LIB ${STATIC_LIBS})
				message("${MESSAGES_INDENTATION}    Found ${STATIC_LIB}")
				setup_file(${STATIC_LIB} ${LIBRARY_SOURCE} ${LIBRARY_DEST})
			endforeach()
		endif()

		if(EXISTS ${THIRD_PARTY_DIR}/include)
			add_module_include_dirs(${THIRD_PARTY_DIR}/include)
		endif()

	endfunction()

#	load_third_party function

	function(load_third_party LIBRARY_PATH)
		set(THIRD_PARTY_DIR ${THIRD_PARTY}/${LIBRARY_PATH})

		if(NOT EXISTS ${THIRD_PARTY_DIR})
			if(NOT EXISTS "${THIRD_PARTY_DIR}.tar.gz")
				message(FATAL_ERROR "Can't find the archive with third-party library '${LIBRARY_PATH}'!")
			endif()

			message(STATUS "Extract third-party library '${LIBRARY_PATH}' from ${THIRD_PARTY_DIR}.tar.gz")
			execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz "${LIBRARY_PATH}.tar.gz" WORKING_DIRECTORY "${THIRD_PARTY}")

			if(NOT EXISTS ${THIRD_PARTY_DIR})
				message(FATAL_ERROR "Can't extract the archive with third-party library '${LIBRARY_PATH}'!")
			endif()
		endif()

		if("${ARGV1}" STREQUAL "COMPONENT" AND NOT "${ARGV2}" STREQUAL "")
			set(COMPONENT_NAME ${ARGV2})
			message("${MESSAGES_INDENTATION}  Depends on ${LIBRARY_PATH}:${COMPONENT_NAME}")
			set(PATH_SUFFIX ${COMPONENT_NAME}/${MODULE_ARCH})
		else()
			message("${MESSAGES_INDENTATION}  Depends on ${LIBRARY_PATH}")
			set(PATH_SUFFIX ${MODULE_ARCH})
		endif()

		set(BINARY_SOURCE ${THIRD_PARTY_DIR}/bin/${PATH_SUFFIX})
		set(LIBRARY_SOURCE ${THIRD_PARTY_DIR}/lib/${PATH_SUFFIX})

		set(BINARY_DEST ${RAPTURE_ROOT}/bin/${MODULE_ARCH})
		set(LIBRARY_DEST ${RAPTURE_ROOT}/lib/${MODULE_ARCH})

		if(EXISTS ${BINARY_SOURCE})
			file(GLOB_RECURSE SHARED_LIBS RELATIVE ${BINARY_SOURCE}
				${BINARY_SOURCE}/*${CMAKE_SHARED_LIBRARY_SUFFIX}
				${BINARY_SOURCE}/*.manifest
				${BINARY_SOURCE}/*.pdb
			)

			foreach(SHARED_LIB ${SHARED_LIBS})
				get_filename_component(SHARED_LIB_NAME ${SHARED_LIB} NAME)
				get_filename_component(SHARED_LIB_DIR ${SHARED_LIB} DIRECTORY)

				setup_file(${SHARED_LIB_NAME} ${BINARY_SOURCE}/${SHARED_LIB_DIR} ${BINARY_DEST}/${SHARED_LIB_DIR}/release)
				setup_file(${SHARED_LIB_NAME} ${BINARY_SOURCE}/${SHARED_LIB_DIR} ${BINARY_DEST}/${SHARED_LIB_DIR}/minsize)
				setup_file(${SHARED_LIB_NAME} ${BINARY_SOURCE}/${SHARED_LIB_DIR} ${BINARY_DEST}/${SHARED_LIB_DIR}/withdebinfo)
				setup_file(${SHARED_LIB_NAME} ${BINARY_SOURCE}/${SHARED_LIB_DIR} ${BINARY_DEST}/${SHARED_LIB_DIR}/debug)
			endforeach()
		endif()

		if(EXISTS ${LIBRARY_SOURCE})
			file(GLOB_RECURSE STATIC_LIBS RELATIVE ${LIBRARY_SOURCE}
				${LIBRARY_SOURCE}/*${CMAKE_STATIC_LIBRARY_SUFFIX}
			)

			foreach(STATIC_LIB ${STATIC_LIBS})
				message("${MESSAGES_INDENTATION}    Found ${STATIC_LIB}")
				setup_file(${STATIC_LIB} ${LIBRARY_SOURCE} ${LIBRARY_DEST})

				string(REGEX REPLACE "[^a-zA-Z0-9_-]" _ LIBRARY_NAME ${STATIC_LIB})

				add_library(${LIBRARY_NAME} STATIC IMPORTED)
				set_target_properties(${LIBRARY_NAME} PROPERTIES IMPORTED_LOCATION ${LIBRARY_DEST}/${STATIC_LIB})

				if("${STATIC_LIB}" MATCHES "debug/")
					target_link_libraries(${PROJECT_NAME} PUBLIC debug ${LIBRARY_NAME})
				elseif("${STATIC_LIB}" MATCHES "release/")
					target_link_libraries(${PROJECT_NAME} PUBLIC optimized ${LIBRARY_NAME})
				else()
					target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARY_NAME})
				endif()
			endforeach()

			file(GLOB_RECURSE STATIC_DEBUG_DATABASES RELATIVE ${LIBRARY_SOURCE}
				${LIBRARY_SOURCE}/*.pdb
			)

			foreach(DEBUG_DB ${STATIC_DEBUG_DATABASES})
				setup_file(${DEBUG_DB} ${LIBRARY_SOURCE} ${BINARY_DEST})
			endforeach()
		endif()

		if(EXISTS ${THIRD_PARTY_DIR}/include)
			add_module_include_dirs(${THIRD_PARTY_DIR}/include)
		endif()
	endfunction()
endif()