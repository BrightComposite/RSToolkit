#--------------------------------------------------------
#	ASD cmake vendor facilities
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

include(CMakeParseArguments)

#--------------------------------------------------------

if(WIN32)
	set(ZIP_PROGRAMS "7z" "jar")
else()
	set(ZIP_PROGRAMS "zip" "jar" "7z")
endif()

set(ZIP_PATH_7z "$ENV{ProgramFiles}/7-Zip")
set(ZIP_PACK_7z "7z")
set(ZIP_UNPACK_7z "7z")
set(ZIP_PACK_FLAGS_7z "a" "-tzip")
set(ZIP_UNPACK_FLAGS_7z "x" "-tzip")

set(ZIP_PATH_jar "$ENV{JDKroot}/bin")
set(ZIP_PACK_jar "jar")
set(ZIP_UNPACK_jar "jar")
set(ZIP_PACK_FLAGS_jar "cf")
set(ZIP_UNPACK_FLAGS_jar "xf")

set(ZIP_PATH_zip "/usr/bin")
set(ZIP_PACK_zip "zip")
set(ZIP_UNPACK_zip "unzip")
set(ZIP_PACK_FLAGS_zip "-r" "-9" "-q")
set(ZIP_UNPACK_FLAGS_zip "")

foreach(zip ${ZIP_PROGRAMS})
	if (NOT ZIP_FOUND)
		find_program(ZIP_FOUND_${ZIP_PACK_${zip}} "${ZIP_PACK_${zip}}" PATHS "${ZIP_PATH_${zip}}" ENV "PATH")
		find_program(ZIP_FOUND_${ZIP_UNPACK_${zip}} "${ZIP_UNPACK_${zip}}" PATHS "${ZIP_PATH_${zip}}" ENV "PATH")
		if (ZIP_FOUND_${ZIP_PACK_${zip}} AND ZIP_FOUND_${ZIP_UNPACK_${zip}})
			set(ZIP_FOUND "${zip}")
		endif (ZIP_FOUND_${ZIP_PACK_${zip}} AND ZIP_FOUND_${ZIP_UNPACK_${zip}})
	endif (NOT ZIP_FOUND)
endforeach()

if(NOT ZIP_FOUND)
	message(FATAL_ERROR "Failed to find zip program (7z, jar, or zip/unzip)")
endif()

function(zip ARCHIVE WORKING_DIRECTORY)
    cmake_parse_arguments(ZIP "" "RESULT_VARIABLE" "" ${ARGN})

    execute_process(COMMAND "${ZIP_FOUND_${ZIP_PACK_${ZIP_FOUND}}}" ${ZIP_PACK_FLAGS_${ZIP_FOUND}} "${ARCHIVE}" ${ARGN} WORKING_DIRECTORY "${WORKING_DIRECTORY}" RESULT_VARIABLE rv)

    if (NOT "${ZIP_RESULT_VARIABLE}" STREQUAL "")
        set(${ZIP_RESULT_VARIABLE} ${rv} PARENT_SCOPE)
    endif()
endfunction()

function(unzip ARCHIVE WORKING_DIRECTORY)
    cmake_parse_arguments(ZIP "" "RESULT_VARIABLE" "" ${ARGN})

    execute_process(COMMAND "${ZIP_FOUND_${ZIP_UNPACK_${ZIP_FOUND}}}" ${ZIP_UNPACK_FLAGS_${ZIP_FOUND}} "${ARCHIVE}" ${ARGN} WORKING_DIRECTORY "${WORKING_DIRECTORY}" RESULT_VARIABLE rv)

    if (NOT "${ZIP_RESULT_VARIABLE}" STREQUAL "")
        set(${ZIP_RESULT_VARIABLE} ${rv} PARENT_SCOPE)
    endif()
endfunction()