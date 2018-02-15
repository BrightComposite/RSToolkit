#--------------------------------------------------------
#	vendor setup file
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

message("${MESSAGES_INDENTATION}  > Depends on glew")

if (WIN32)
    set(ROOT ${CMAKE_CURRENT_LIST_DIR}/glew-2.0.0)

    if (NOT EXISTS "${ROOT}")
        include(${ASD_TOOLS}/zip.cmake)

        set(ARCHIVE "${CMAKE_CURRENT_LIST_DIR}/glew-2.0.0-win32.zip")

        file(
            DOWNLOAD
            https://github.com/nigels-com/glew/releases/download/glew-2.0.0/glew-2.0.0-win32.zip
            "${ARCHIVE}"
            STATUS status
            LOG log
            )

        list(GET status 0 status_code)
        list(GET status 1 status_string)

        if(NOT status_code EQUAL 0)
            message(FATAL_ERROR "Downloading GLEW failed
                status_code: ${status_code}
                status_string: ${status_string}
                log: ${log}
                ")
        endif()

        unzip("${ARCHIVE}" "${CMAKE_CURRENT_LIST_DIR}" RESULT_VARIABLE rv)

        file(REMOVE "${ARCHIVE}")

        if(NOT rv EQUAL 0)
            message(FATAL_ERROR "Can't extract GLEW binaries")
        endif()
    endif()

    set(LIB glew32.lib)

    if ("${MODULE_ARCH}" STREQUAL "x64")
        set(LIBRARY "${ROOT}/lib/Release/x64/${LIB}")
		file(GLOB DLLS "${ROOT}/bin/Release/x64/*.dll")
    elseif("${MODULE_ARCH}" STREQUAL "x86")
        set(LIBRARY "${ROOT}/lib/Release/Win32/${LIB}")
		file(GLOB DLLS "${ROOT}/bin/Release/Win32/*.dll")
    else()
        message(FATAL_ERROR "GLEW: Unsupported architecture!")
    endif()

    file(COPY ${DLLS} DESTINATION "${OUTPUT_ROOT}/bin/release")
    file(COPY ${DLLS} DESTINATION "${OUTPUT_ROOT}/bin/debug")
elseif(UNIX)
    set(ROOT ${CMAKE_CURRENT_LIST_DIR}/code)

    if (NOT EXISTS "${ROOT}")
        git_clone("https://github.com/nigels-com/glew" code ${CMAKE_CURRENT_LIST_DIR} TAG "glew-2.0.0")
    endif()

    set(OUTDIR ${CMAKE_CURRENT_LIST_DIR}/output)

    set(LIBRARY ${OUTDIR}/lib/Release/libglew32.a)

    if(NOT EXISTS ${LIBRARY})
        file(MAKE_DIRECTORY ${OUTDIR})

        # NOTE: needed to comment `target_link_libraries (glew LINK_PRIVATE -nodefaultlib -noentry)` in code/build/cmake/CmakeLists.txt:103

        execute_process(COMMAND ${CMAKE_COMMAND} --target glew_s -G "${CMAKE_GENERATOR}" -DCMAKE_BUILD_TYPE=Release ${ROOT}/build/cmake WORKING_DIRECTORY ${OUTDIR})
        execute_process(COMMAND ${CMAKE_COMMAND} --build ${OUTDIR} --config Release)
    endif()

    if(NOT EXISTS ${LIBRARY})
        message(FATAL_ERROR "Can't find glew at ${LIBRARY}")
    endif()
else()
    message(FATAL_ERROR "Unsupported platform! Needed to implement")
endif()

#--------------------------------------------------------

add_module_include_dirs(${ROOT}/include)
add_module_libraries(${LIBRARY})

if (WIN32)
    add_module_libraries("opengl32.lib" "gdi32.lib" "msimg32.lib" "winmm.lib")
endif ()

#--------------------------------------------------------
