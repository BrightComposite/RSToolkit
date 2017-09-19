#--------------------------------------------------------
#	ASD cmake utils
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

if(NOT ";${GUARD_BLOCKS};" MATCHES ";UTILS_GUARD;")
	set(GUARD_BLOCKS ${GUARD_BLOCKS};UTILS_GUARD CACHE INTERNAL "Guard blocks" FORCE)

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

	function(escape_regular OUT_STR STR)
		string(REPLACE "\\" "\\\\" BUF "${STR}")
		string(REPLACE "/" "\\/" BUF "${BUF}")
		string(REPLACE "+" "\\+" BUF "${BUF}")
		string(REPLACE "." "\\." BUF "${BUF}")
		string(REPLACE ":" "\\:" BUF "${BUF}")

		set(${OUT_STR} ${BUF} PARENT_SCOPE)
	endfunction()

	function(color_message color message)
		if(UNIX)
			string(TOLOWER ${color} color)
			execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --${color} --bold ${message})
		else()
			message(${message})
		endif()
	endfunction()

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

		set(TEMPLATE_FILE ${ASD_TOOLS}/templates/${filename}${SRC_EXT})
		set(TEMPLATE_FILE_EXT ${ASD_TOOLS}/templates/_${SRC_EXT})

		message(STATUS "Create new source file ${SRC_FULLPATH}...")

		if(EXISTS ${TEMPLATE_FILE})
			configure_file(${TEMPLATE_FILE} ${SRC_FULLPATH})
		elseif(EXISTS ${TEMPLATE_FILE_EXT})
			configure_file(${TEMPLATE_FILE_EXT} ${SRC_FULLPATH})
		else()
			file(WRITE ${SRC_FULLPATH} "")
		endif()
	endfunction()

	function(setup_file FILE_PATH SOURCE DEST)
		if(NOT EXISTS ${DEST}/${FILE_PATH})
			get_filename_component(FILE_DIR ${FILE_PATH} DIRECTORY)
			file(COPY ${SOURCE}/${FILE_PATH} DESTINATION ${DEST}/${FILE_DIR})
		endif()
	endfunction()
		
	function(join VALUES GLUE OUTPUT)
		string (REGEX REPLACE "([^\\]|^);" "\\1${GLUE}" _TMP_STR "${VALUES}")
		string (REGEX REPLACE "[\\](.)" "\\1" _TMP_STR "${_TMP_STR}") # fixes escaping
		set(${OUTPUT} "${_TMP_STR}" PARENT_SCOPE)
	endfunction()
endif()
