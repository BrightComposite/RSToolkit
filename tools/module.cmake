#--------------------------------------------------------
#	Rapture State cmake project facilities
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.0)

if(WIN32)
	set(CMAKE_SHARED_LIBRARY_PREFIX "" CACHE INTERNAL "Cmake shared lib prefix")
endif ()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(MODULE_ARCH x64 CACHE INTERNAL "Project architecture")
else()
	set(MODULE_ARCH x86 CACHE INTERNAL "Project architecture")
endif()

#--------------------------------------------------------

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

set(MODULE_TYPES APPLICATION;SHARED;LIBRARY;INLINE CACHE INTERNAL "Module types" FORCE)

if(NOT ";${GUARD_BLOCKS};" MATCHES ";PROJECT_TOOL_GUARD;")
	set(GUARD_BLOCKS ${GUARD_BLOCKS};PROJECT_TOOL_GUARD CACHE INTERNAL "Guard blocks" FORCE)

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

#	set_module_sources function.
#	Usage:
#		set_module_sources(<output list>
#			SRC_GROUP <subdirectory> <source group section>
#				START_SECTION <relative path>
#					[filename...]
#				END_SECTION
#				[START_SECTION <relative path> [filename...] END_SECTION ...]
#			[SRC_GROUP <subdirectory> <source group section>
#				...
#			]
#		)

	function(set_module_sources OUT_LIST)
		set(FULL_SRC_LIST)

		set(SECTION_POS 0)
		set(SECTION_PATH "")
		set(SUBSECTIONS 0)

		set(GROUP_POS 0)
		set(GROUP_ROOT "")
		set(GROUP_TYPE "")

		set(ROOT_DIR ${PROJECT_SOURCE_DIR})

		foreach(SRC_ENTRY ${ARGN})
			if(${SRC_ENTRY} STREQUAL "START_SECTION" OR ${SRC_ENTRY} STREQUAL "END_SECTION" OR ${SRC_ENTRY} STREQUAL "SRC_GROUP") # Check if new section or group is defined here

				# Check correctness of function usage

				if(${SRC_ENTRY} STREQUAL "START_SECTION")
					if(${SECTION_POS} GREATER 0)
						message(FATAL_ERROR "Incorrect usage of 'add_module_sources' function! The new section is defined before the end of the previous section header!")
					elseif(${GROUP_POS} GREATER 0)
						message(FATAL_ERROR "Incorrect usage of 'add_module_sources' function! The new section is defined before the end of group header!")
					endif()

					set(SECTION_POS 1)
					math(EXPR SUBSECTIONS "${SUBSECTIONS} + 1")

				elseif(${SRC_ENTRY} STREQUAL "SRC_GROUP")
					if(${SUBSECTIONS} GREATER 0)
						message(FATAL_ERROR "Incorrect usage of 'add_module_sources' function! Can't declare next source group inside subsection!")
					elseif(${GROUP_POS} GREATER 0)
						message(FATAL_ERROR "Incorrect usage of 'add_module_sources' function! The new group is defined before the end of the previous group header!")
					elseif(${SECTION_POS} GREATER 0)
						message(FATAL_ERROR "Incorrect usage of 'add_module_sources' function! The new group is defined before the end of section header!")
					endif()

					set(GROUP_POS 1)
					set(SECTION_PATH "")
					set(SUBSECTIONS 0)

				elseif(${SRC_ENTRY} STREQUAL "END_SECTION")
					if(${SUBSECTIONS} EQUAL 0)
						message(FATAL_ERROR "Incorrect usage of 'add_module_sources' function! Unexpected end of section!")
					endif()

					list(LENGTH SRC_LIST_${SUBSECTIONS} SRC_COUNT)

					if(NOT SRC_COUNT EQUAL 0)
						if("${SECTION_PATH}" STREQUAL "" OR "${SECTION_PATH}" STREQUAL "/")
							source_group(${GROUP_TYPE} FILES ${SRC_LIST_${SUBSECTIONS}})
						else()
							string(REPLACE / \\ GROUP_PATH ${SECTION_PATH})
							source_group(${GROUP_TYPE}\\${GROUP_PATH} FILES ${SRC_LIST_${SUBSECTIONS}})
						endif()

						set(FULL_SRC_LIST ${FULL_SRC_LIST} ${SRC_LIST_${SUBSECTIONS}})
						set(SRC_LIST_${SUBSECTIONS})
					endif()

					math(EXPR SUBSECTIONS "${SUBSECTIONS} - 1")
					get_filename_component(SECTION_PATH "${SECTION_PATH}" DIRECTORY)
				endif()
			elseif(${SECTION_POS} EQUAL 1)	# Check if we are reading section path
				if("${SECTION_PATH}" STREQUAL "" OR "${SECTION_PATH}" STREQUAL "/")
					set(SECTION_PATH ${SRC_ENTRY})
				else()
					set(SECTION_PATH ${SECTION_PATH}/${SRC_ENTRY})
				endif()

				if(${SECTION_PATH} STREQUAL SRC_ROOT)
					set(SECTION_PATH "")
				endif()

				set(SECTION_POS 0)

			elseif(${GROUP_POS} EQUAL 1)	# Check if we are reading group root
				set(GROUP_ROOT ${SRC_ENTRY})
				set(GROUP_POS 2)

			elseif(${GROUP_POS} EQUAL 2) 	# Check if we are reading group type
				set(GROUP_TYPE ${SRC_ENTRY})
				set(GROUP_POS 0)

			else() # Add new source to the current group and section
				if(NOT ${SECTION_PATH} STREQUAL "")
					set(NEW_SOURCE ${SECTION_PATH}/${SRC_ENTRY})
				else()
					set(NEW_SOURCE ${SRC_ENTRY})
				endif()

				set(SRC_ROOT ${ROOT_DIR}/${GROUP_ROOT})

				if(NOT EXISTS ${SRC_ROOT}/${NEW_SOURCE})
					create_source(${SRC_ROOT} ${NEW_SOURCE})
				endif()

				set(SRC_LIST_${SUBSECTIONS} ${SRC_LIST_${SUBSECTIONS}} ${SRC_ROOT}/${NEW_SOURCE})
			endif()
		endforeach()

		if(${SUBSECTIONS} GREATER 0)
			message(FATAL_ERROR "Incorrect usage of 'add_module_sources' function! End of section was not found!")
		endif()

		list(LENGTH SRC_LIST_${SUBSECTIONS} SRC_COUNT)

		if(NOT SRC_COUNT EQUAL 0)
			if(NOT ${SECTION_PATH} STREQUAL "")
				string(REPLACE / \\ GROUP_PATH ${SECTION_PATH})
				source_group(${GROUP_TYPE}\\${GROUP_PATH} FILES ${SRC_LIST_${SUBSECTIONS}})
			else()
				source_group(${GROUP_TYPE} FILES ${SRC_LIST_${SUBSECTIONS}})
			endif()

			list(APPEND FULL_SRC_LIST ${SRC_LIST_${SUBSECTIONS}})
			set(SRC_LIST_${SUBSECTIONS})
		endif()

		set(${OUT_LIST} ${FULL_SRC_LIST} PARENT_SCOPE)
	endfunction()

#	add_module_sources function.
#	Usage:
#		add_module_sources(
#			SRC_GROUP <subdirectory> <source group section>
#				START_SECTION <relative path>
#					[filename...]
#				END_SECTION
#				[START_SECTION <relative path> [filename...] END_SECTION ...]
#			[SRC_GROUP <subdirectory> <source group section>
#				...
#			]
#		)

	function(add_module_sources)
		set_module_sources(OUT_LIST ${ARGN})
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

#	require_modules function.

	function(require_modules)
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

#	start_module function.

	function(start_module type)
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

		if(NOT "${ARGN}" STREQUAL "")
			require_modules(${ARGN})
		endif()
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

#	set_module_api_key function.

	function(set_module_api_key key)
		set(${PROJECT_NAME}_API_KEY ${key} CACHE STRING "${PROJECT_NAME} api key" FORCE)
	endfunction()

#	set_module_api_export function.

	function(set_module_api_export MODULE)
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

#	set_module_api_import function.

	function(set_module_api_import MODULE)
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

#	end_module function.

	function(end_module)
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
		target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS})

		add_module_include_dirs(${PROJECT_SOURCE_DIR}/include)
		add_module_libraries(${${PROJECT_NAME}_MODULE_DEPENDENCIES})
		set_module_api_export(${PROJECT_NAME})

		collect_dependencies(DEPENDENCIES ${PROJECT_NAME})

		foreach(DEPENDENCY_NAME ${DEPENDENCIES})
			if("${${DEPENDENCY_NAME}_MODULE_TYPE}" STREQUAL "SHARED")
				set_module_api_import(${DEPENDENCY_NAME})
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