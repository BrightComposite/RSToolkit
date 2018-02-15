#--------------------------------------------------------
#	vendor setup file
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

include(${ASD_TOOLS}/git.cmake)

#--------------------------------------------------------

message("${MESSAGES_INDENTATION}  > Depends on zlib")

set(ZLIB_ROOT "" CACHE PATH "Zlib root" FORCE)

find_package(ZLIB)

if (ZLIB_FOUND AND EXISTS ${ZLIB_INCLUDE_DIRS})
    message("${MESSAGES_INDENTATION}  * Found zlib libraries ${ZLIB_LIBRARIES}")

	target_link_libraries(${PROJECT_NAME} PRIVATE ${ZLIB_LIBRARIES})
	include_directories(${ZLIB_INCLUDE_DIRS})
else ()
    if(WIN32)
        set(LIB zlib.lib)
    elseif(UNIX)
        set(LIB libzlib.a)
    else()
        message(FATAL_ERROR "Unsupported platform! Needed to implement")
    endif()

    set(ROOT ${CMAKE_CURRENT_LIST_DIR}/code)

    if (NOT EXISTS "${ROOT}")
        git_clone("https://github.com/madler/zlib.git" code ${CMAKE_CURRENT_LIST_DIR})
    endif()

    set(BUILDDIR ${BUILD_ROOT}/vendor/zlib)
    set(OUTDIR ${OUTPUT_ROOT}/vendor/zlib)

    set(LIBRARY ${OUTDIR}/lib/${LIB})

    if(NOT EXISTS ${LIBRARY})
        if(NOT EXISTS ${BUILDDIR})
            file(MAKE_DIRECTORY ${BUILDDIR})
        endif()

        if(NOT EXISTS ${OUTDIR})
            file(MAKE_DIRECTORY ${OUTDIR})
        endif()

        execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -DCMAKE_INSTALL_PREFIX=${OUTDIR} -DCMAKE_BUILD_TYPE=Release ${ROOT} WORKING_DIRECTORY ${BUILDDIR})
        execute_process(COMMAND ${CMAKE_COMMAND} --build ${BUILDDIR} --config Release --target INSTALL)
    endif()

    set(ZLIB_ROOT "${OUTDIR}" CACHE PATH "Zlib root" FORCE)
    find_package(ZLIB REQUIRED)

    target_link_libraries(${PROJECT_NAME} PRIVATE ${ZLIB_LIBRARIES})
    include_directories(${ZLIB_INCLUDE_DIRS})
endif ()
