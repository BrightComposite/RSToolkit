#--------------------------------------------------------
#	Rapture State cmake project facilities
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.0)

if(WIN32)
	set(CMAKE_SHARED_LIBRARY_PREFIX "" CACHE INTERNAL "Cmake shared lib prefix")
endif ()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(PROJECT_ARCHITECTURE x64 CACHE INTERNAL "Project architecture")
else()
	set(PROJECT_ARCHITECTURE x86 CACHE INTERNAL "Project architecture")
endif()

#--------------------------------------------------------

set(OUTPUT_ROOT "${RAPTURE_ROOT}/build-${PROJECT_ARCHITECTURE}" CACHE INTERNAL "Output root")
set(MODULES_ROOT "${RAPTURE_ROOT}/modules" CACHE INTERNAL "Modules root")

set(BINARY_OUTPUT ${RAPTURE_ROOT}/bin/${PROJECT_ARCHITECTURE} CACHE INTERNAL "Binary output")
set(LIBRARY_OUTPUT ${RAPTURE_ROOT}/bin/${PROJECT_ARCHITECTURE} CACHE INTERNAL "Library output")
set(THIRD_PARTY ${RAPTURE_ROOT}/third-party CACHE INTERNAL "Third-party directory")

function(set_output_dir DIR TYPE)
	get_filename_component(REAL_DIR ${DIR} REALPATH)
	set(OUTPUT_DIR CMAKE_${TYPE}_OUTPUT_DIRECTORY)
	
	set(${OUTPUT_DIR} ${REAL_DIR}/${PROJECT_ARCHITECTURE} CACHE INTERNAL "Cmake ${TYPE} output")
	
	set(${OUTPUT_DIR}_DEBUG ${${OUTPUT_DIR}}/debug CACHE INTERNAL "Cmake ${TYPE} debug output")
	set(${OUTPUT_DIR}_RELEASE ${${OUTPUT_DIR}}/release CACHE INTERNAL "Cmake ${TYPE} release output")
	set(${OUTPUT_DIR}_MINSIZEREL ${${OUTPUT_DIR}}/minsize CACHE INTERNAL "Cmake ${TYPE} minsizerelease output")
	set(${OUTPUT_DIR}_RELWITHDEBINFO ${${OUTPUT_DIR}}/withdebinfo CACHE INTERNAL "Cmake ${TYPE} relwithdebinfo output")
endfunction()

set_output_dir(${RAPTURE_ROOT}/lib ARCHIVE)
set_output_dir(${RAPTURE_ROOT}/lib LIBRARY)
set_output_dir(${RAPTURE_ROOT}/bin RUNTIME)

link_directories(${RAPTURE_ROOT}/lib/${PROJECT_ARCHITECTURE})

if(NOT ";${GUARD_BLOCKS};" MATCHES ";PROJECT_TOOL_GUARD;")
	set(GUARD_BLOCKS ${GUARD_BLOCKS};PROJECT_TOOL_GUARD CACHE INTERNAL "Guard blocks" FORCE)

#--------------------------------------------------------
#	Functions
#--------------------------------------------------------

#	create_project_source function.

	function(create_project_source SRC_ROOT SRC_PATH)
		set(SRC_FULLPATH ${SRC_ROOT}/${SRC_PATH})
		
		get_filename_component(SRC_DIR  ${SRC_FULLPATH} DIRECTORY)
		get_filename_component(SRC_EXT  ${SRC_FULLPATH} EXT)
		get_filename_component(SRC_SHORT_DIR  ${SRC_PATH} DIRECTORY)
		
		get_filename_component(filename ${SRC_FULLPATH} NAME_WE)
		set(short_path ${SRC_SHORT_DIR}/${filename})
		
		if(NOT EXISTS ${SRC_DIR})
			file(MAKE_DIRECTORY ${SRC_DIR})
		endif()
		
		string(REGEX REPLACE "([A-Z])" "_\\1" FILE_NAME ${filename})
		string(REGEX REPLACE "^_(.*)$" "\\1" FILE_NAME ${FILE_NAME})
		string(TOUPPER ${FILE_NAME} FILE_NAME)
		
		set(TEMPLATE_FILE ${RAPTURE_ROOT}/templates/template${SRC_EXT})
		set(TEMPLATE_FILE2 ${RAPTURE_ROOT}/templates/template.${filename}${SRC_EXT})
		
		if(EXISTS ${TEMPLATE_FILE})
			configure_file(${TEMPLATE_FILE} ${SRC_FULLPATH})
		elseif(EXISTS ${TEMPLATE_FILE2})
			configure_file(${TEMPLATE_FILE2} ${SRC_FULLPATH})
		else()
			file(WRITE ${SRC_FULLPATH} "")
		endif()
	endfunction()
	
#	set_project_sources function.
#	Usage:
#		set_project_sources(<output list> <root project directory>
#			SRC_GROUP <subdirectory> <source group section>
#				START_SECTION <relative path>
#					[filename...]
#				END_SECTION
#				[START_SECTION <relative path> [filename...] END_SECTION ...]
#			[SRC_GROUP <subdirectory> <source group section>
#				...
#			]
#		)

	function(set_project_sources OUT_LIST ROOT_DIR)
		set(FULL_SRC_LIST)
		
		set(SECTION_POS 0)
		set(SECTION_PATH "")
		set(SUBSECTIONS 0)
		
		set(GROUP_POS 0)
		set(GROUP_ROOT "")
		set(GROUP_TYPE "")
		
		foreach(SRC_ENTRY ${ARGN})
			if(${SRC_ENTRY} STREQUAL "START_SECTION" OR ${SRC_ENTRY} STREQUAL "END_SECTION" OR ${SRC_ENTRY} STREQUAL "SRC_GROUP") # Check if new section or group is defined here
				
				# Check correctness of function usage
				
				if(${SRC_ENTRY} STREQUAL "START_SECTION")
					if(${SECTION_POS} GREATER 0)
						message(FATAL_ERROR "Incorrect usage of 'set_project_sources' function! The new section is defined before the end of the previous section header!")
					elseif(${GROUP_POS} GREATER 0)
						message(FATAL_ERROR "Incorrect usage of 'set_project_sources' function! The new section is defined before the end of group header!")
					endif()
					
					set(SECTION_POS 1)
					math(EXPR SUBSECTIONS "${SUBSECTIONS} + 1")
					
				elseif(${SRC_ENTRY} STREQUAL "SRC_GROUP")
					if(${SUBSECTIONS} GREATER 0)
						message(FATAL_ERROR "Incorrect usage of 'set_project_sources' function! Can't declare next source group inside subsection!")
					elseif(${GROUP_POS} GREATER 0)
						message(FATAL_ERROR "Incorrect usage of 'set_project_sources' function! The new group is defined before the end of the previous group header!")
					elseif(${SECTION_POS} GREATER 0)
						message(FATAL_ERROR "Incorrect usage of 'set_project_sources' function! The new group is defined before the end of section header!")
					endif()
					
					set(GROUP_POS 1)
					set(SECTION_PATH "")
					set(SUBSECTIONS 0)
					
				elseif(${SRC_ENTRY} STREQUAL "END_SECTION")
					if(${SUBSECTIONS} EQUAL 0)
						message(FATAL_ERROR "Incorrect usage of 'set_project_sources' function! Unexpected end of section!")
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
					create_project_source(${SRC_ROOT} ${NEW_SOURCE})
				endif()
		
				set(SRC_LIST_${SUBSECTIONS} ${SRC_LIST_${SUBSECTIONS}} ${SRC_ROOT}/${NEW_SOURCE})
			endif()
		endforeach()
		
		if(${SUBSECTIONS} GREATER 0)
			message(FATAL_ERROR "Incorrect usage of 'set_project_sources' function! End of section was not found!")
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
	
#	set_project_include_dirs function.

	function(set_project_include_dirs)
		target_include_directories(${PROJECT_NAME} PUBLIC ${ARGN})
	endfunction()

#	set_dependencies function.
	
	function(set_dependencies)
		set(${PROJECT_NAME}_DEPENDENCIES ${ARGN}
			CACHE INTERNAL "${PROJECT_NAME}: Dependencies" FORCE
		)
	endfunction()
	
#	start_module function.
	
	function(start_module)
		if(NOT ";${GUARD_BLOCKS};" MATCHES ";${PROJECT_NAME}_GUARD;")
			set(GUARD_BLOCKS ${GUARD_BLOCKS};${PROJECT_NAME}_GUARD CACHE INTERNAL "Guard blocks" FORCE)
		endif()
	endfunction()
	
#	require_module function.
	
	function(require_module name path)
		if(NOT ";${GUARD_BLOCKS};" MATCHES ";${name}_GUARD;")
			add_subdirectory("${MODULES_ROOT}/${path}" "${OUTPUT_ROOT}/${path}")
		endif()
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
	
#	setup_file function

	function(setup_file FILE_PATH SOURCE DEST)
		if(NOT EXISTS ${DEST}/${FILE_PATH})
			get_filename_component(FILE_DIR ${FILE_PATH} DIRECTORY)
			file(COPY ${SOURCE}/${FILE_PATH} DESTINATION ${DEST}/${FILE_DIR})
		endif()
	endfunction()
	
#	target_setup_third_party function

	function(target_setup_third_party TARGET LIBRARY_PATH)
		set(THIRD_PARTY_DIR ${THIRD_PARTY}/${LIBRARY_PATH})
	
		if("${ARGV2}" STREQUAL "COMPONENT" AND NOT "${ARGV3}" STREQUAL "")
			set(PATH_SUFFIX ${ARGV3}/${PROJECT_ARCHITECTURE})
		else()
			set(PATH_SUFFIX ${PROJECT_ARCHITECTURE})
		endif()
		
		set(BINARY_SOURCE ${THIRD_PARTY_DIR}/bin/${PATH_SUFFIX})
		set(LIBRARY_SOURCE ${THIRD_PARTY_DIR}/lib/${PATH_SUFFIX})
		
		set(BINARY_DEST ${RAPTURE_ROOT}/bin/${PROJECT_ARCHITECTURE})
		set(LIBRARY_DEST ${RAPTURE_ROOT}/lib/${PROJECT_ARCHITECTURE})
		
		if(EXISTS ${BINARY_SOURCE})
			file(GLOB_RECURSE SHARED_LIBS RELATIVE ${BINARY_SOURCE}
				${BINARY_SOURCE}/*${CMAKE_SHARED_LIBRARY_SUFFIX}
			)
			
			foreach(SHARED_LIB ${SHARED_LIBS})
				get_filename_component(SHARED_LIB_NAME ${SHARED_LIB} NAME)
				get_filename_component(SHARED_LIB_DIR ${SHARED_LIB} DIRECTORY)
				
				setup_file(${SHARED_LIB_NAME} ${BINARY_SOURCE}/${SHARED_LIB_DIR} ${BINARY_DEST}/${SHARED_LIB_DIR}/release)
				setup_file(${SHARED_LIB_NAME} ${BINARY_SOURCE}/${SHARED_LIB_DIR} ${BINARY_DEST}/${SHARED_LIB_DIR}/debug)
			endforeach()
		endif()
	
		if(EXISTS ${LIBRARY_SOURCE})
			file(GLOB_RECURSE STATIC_LIBS RELATIVE ${LIBRARY_SOURCE}
				${LIBRARY_SOURCE}/*${CMAKE_STATIC_LIBRARY_SUFFIX}
			)
			
			foreach(STATIC_LIB ${STATIC_LIBS})
				setup_file(${STATIC_LIB} ${LIBRARY_SOURCE} ${LIBRARY_DEST})
			endforeach()
		endif()
			
		if(EXISTS ${THIRD_PARTY_DIR}/include)
			target_include_directories(${TARGET} PUBLIC ${THIRD_PARTY_DIR}/include)
		endif()
		
	endfunction()
	
#	target_load_third_party function

	function(target_load_third_party TARGET LIBRARY_PATH)
		set(THIRD_PARTY_DIR ${THIRD_PARTY}/${LIBRARY_PATH})
	
		if("${ARGV2}" STREQUAL "COMPONENT" AND NOT "${ARGV3}" STREQUAL "")
			set(PATH_SUFFIX ${ARGV3}/${PROJECT_ARCHITECTURE})
		else()
			set(PATH_SUFFIX ${PROJECT_ARCHITECTURE})
		endif()
		
		set(BINARY_SOURCE ${THIRD_PARTY_DIR}/bin/${PATH_SUFFIX})
		set(LIBRARY_SOURCE ${THIRD_PARTY_DIR}/lib/${PATH_SUFFIX})
		
		set(BINARY_DEST ${RAPTURE_ROOT}/bin/${PROJECT_ARCHITECTURE})
		set(LIBRARY_DEST ${RAPTURE_ROOT}/lib/${PROJECT_ARCHITECTURE})
		
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
				setup_file(${SHARED_LIB_NAME} ${BINARY_SOURCE}/${SHARED_LIB_DIR} ${BINARY_DEST}/${SHARED_LIB_DIR}/debug)
			endforeach()
		endif()
	
		if(EXISTS ${LIBRARY_SOURCE})
			file(GLOB_RECURSE STATIC_LIBS RELATIVE ${LIBRARY_SOURCE}
				${LIBRARY_SOURCE}/*${CMAKE_STATIC_LIBRARY_SUFFIX}
			)
			
			foreach(STATIC_LIB ${STATIC_LIBS})
				setup_file(${STATIC_LIB} ${LIBRARY_SOURCE} ${LIBRARY_DEST})
				
				string(REGEX REPLACE "[^a-zA-Z0-9_-]" _ LIBRARY_NAME ${STATIC_LIB})
				
				add_library(${LIBRARY_NAME} STATIC IMPORTED)
				set_target_properties(${LIBRARY_NAME} PROPERTIES IMPORTED_LOCATION ${LIBRARY_DEST}/${STATIC_LIB})
				
				if("${STATIC_LIB}" MATCHES "debug/")
					target_link_libraries(${TARGET} PUBLIC debug ${LIBRARY_NAME})
				elseif("${STATIC_LIB}" MATCHES "release/")
					target_link_libraries(${TARGET} PUBLIC optimized ${LIBRARY_NAME})
				else()
					target_link_libraries(${TARGET} PUBLIC ${LIBRARY_NAME})
				endif()
			endforeach()
			
			file(GLOB_RECURSE STATIC_DEBUG_DATABASES RELATIVE ${LIBRARY_SOURCE}
				${LIBRARY_SOURCE}/*.pdb
			)
			
			foreach(DEBUG_DB ${STATIC_DEBUG_DATABASES})
				setup_file(${DEBUG_DB} ${LIBRARY_SOURCE} ${LIBRARY_DEST})
			endforeach()
		endif()
			
		if(EXISTS ${THIRD_PARTY_DIR}/include)
			target_include_directories(${TARGET} PUBLIC ${THIRD_PARTY_DIR}/include)
		endif()
		
	endfunction()
	
endif()