#--------------------------------------------------------
#	vendor setup file
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

vendor(zlib)

if(WIN32)
	set(LIB freetype.lib)
elseif(UNIX)
	set(LIB libfreetype.a)
else()
	message(FATAL_ERROR "Unsupported platform! Needed to implement")
endif()

set(OUTDIR "${OUTPUT_ROOT}/vendor/freetype")

set(LIBRARY "${OUTDIR}/lib/${LIB}")

if(NOT EXISTS ${LIBRARY})
	set(BUILDDIR "${BUILD_ROOT}/vendor/freetype")

	if(NOT EXISTS "${BUILDDIR}")
		file(MAKE_DIRECTORY "${BUILDDIR}")
	endif()

	if(NOT EXISTS "${OUTDIR}")
		file(MAKE_DIRECTORY "${OUTDIR}")
	endif()

	execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -DWITH_ZLIB=ON "${CMAKE_CURRENT_LIST_DIR}/freetype-2.7.1" -DZLIB_ROOT=${ZLIB_ROOT} -DCMAKE_INSTALL_PREFIX=${OUTDIR} WORKING_DIRECTORY "${BUILDDIR}")
	execute_process(COMMAND ${CMAKE_COMMAND} --build "${BUILDDIR}" --config Release --target INSTALL)
endif()

include_directories("${OUTDIR}/include")

target_link_libraries(${PROJECT_NAME} PRIVATE "${LIBRARY}")