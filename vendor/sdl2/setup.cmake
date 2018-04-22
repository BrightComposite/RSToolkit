#--------------------------------------------------------
#	vendor setup file
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

include(${ASD_TOOLS}/git.cmake)

#--------------------------------------------------------

message("${MESSAGES_INDENTATION}  > Depends on sdl2")

set(OUTDIR "${OUTPUT_ROOT}/vendor/sdl2")

if (WIN32)
	set(LIBRARY "${OUTDIR}/lib/${MODULE_ARCH}/SDL2.lib")

	if (NOT EXISTS "${LIBRARY}")
		set(ROOT "${CMAKE_CURRENT_LIST_DIR}/SDL2-2.0.7")

		if (NOT EXISTS "${ROOT}/include")
			include(${ASD_TOOLS}/zip.cmake)

			set(ARCHIVE "${CMAKE_CURRENT_LIST_DIR}/binaries.zip")

			file(
				DOWNLOAD
				"https://libsdl.org/release/SDL2-devel-2.0.7-VC.zip"
				"${ARCHIVE}"
				STATUS status
				LOG log
				)

			list(GET status 0 status_code)
			list(GET status 1 status_string)

			if(NOT status_code EQUAL 0)
				message(FATAL_ERROR "Downloading SDL2 failed
					status_code: ${status_code}
					status_string: ${status_string}
					log: ${log}
					")
			endif()

			unzip("${ARCHIVE}" "${CMAKE_CURRENT_LIST_DIR}" RESULT_VARIABLE rv)

			file(REMOVE "${ARCHIVE}")

			if(NOT rv EQUAL 0)
				message(FATAL_ERROR "Can't extract SDL2 binaries")
			endif()
		endif ()

		file(COPY "${ROOT}/include" DESTINATION "${OUTDIR}/")
		file(COPY "${ROOT}/lib" DESTINATION "${OUTDIR}/")
		file(GLOB DLLS "${OUTDIR}/lib/${MODULE_ARCH}/*.dll")

		file(COPY ${DLLS} DESTINATION "${OUTPUT_ROOT}/bin/release")
		file(COPY ${DLLS} DESTINATION "${OUTPUT_ROOT}/bin/debug")
	endif()

	target_link_libraries(${PROJECT_NAME} PUBLIC "${LIBRARY}")
	add_module_include_dirs("${OUTDIR}/include")
elseif (UNIX)
	set(LIBRARY "${OUTDIR}/lib/libSDL2.a")

	if (NOT EXISTS "${LIBRARY}")
		set(ROOT ${CMAKE_CURRENT_LIST_DIR}/code)

		if (NOT EXISTS "${ROOT}")
			git_clone("https://github.com/davidsiaw/SDL2.git" code ${CMAKE_CURRENT_LIST_DIR})
		endif()

		set(BUILDDIR "${BUILD_ROOT}/vendor/sdl2")

		if (EXISTS "${BUILDDIR}")
			file(REMOVE_RECURSE "${BUILDDIR}")
		endif()

		file(MAKE_DIRECTORY "${BUILDDIR}")

		if (NOT EXISTS "${OUTDIR}")
			file(MAKE_DIRECTORY "${OUTDIR}")
		endif()

		execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -DCMAKE_C_COMPILER=/usr/bin/gcc -DSDL_STATIC=ON -DSDL_SHARED=OFF -DCMAKE_INSTALL_PREFIX=${OUTDIR} "${CMAKE_CURRENT_LIST_DIR}/code" WORKING_DIRECTORY "${BUILDDIR}")
		execute_process(COMMAND ${CMAKE_COMMAND} --build "${BUILDDIR}" --config Release --target INSTALL)
	endif()

	target_link_libraries(${PROJECT_NAME} PUBLIC "${LIBRARY}")
	target_link_libraries(${PROJECT_NAME} PUBLIC -ldl)
	add_module_include_dirs("${OUTDIR}/include/SDL2")
else ()
	message(FATAL_ERROR "Current platform is not supported yet")
endif ()

