#--------------------------------------------------------
#	vendor setup file
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

include(${ASD_TOOLS}/git.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/dependencies.cmake)

#--------------------------------------------------------

set(ROOT ${CMAKE_CURRENT_LIST_DIR}/code)

if (NOT EXISTS "${ROOT}")
    git_clone("https://github.com/boostorg/boost" code ${CMAKE_CURRENT_LIST_DIR})
    git_submodule("${ROOT}" "tools/build")
endif()

git_submodule("${ROOT}" "libs/config")

if("${ARGUMENTS}" STREQUAL "")
	message(FATAL_ERROR "You must specify arguments! Valid arguments are: REINSTALL, COMPONENTS <comp...>")
endif()

cmake_parse_arguments(SETUP "REINSTALL" "" "COMPONENTS" ${ARGUMENTS})

set(BOOST_LIBRARIES CACHE INTERNAL "Boost libraries" FORCE)
set(BOOST_INCLUDES CACHE INTERNAL "Boost includes" FORCE)

set(BOOST_RESOLVED CACHE INTERNAL "Resolved Boost libraries" FORCE)

function(resolve module)
    if(NOT ";${BOOST_RESOLVED};" MATCHES ";${module};")
        message("${MESSAGES_INDENTATION}  > Depends on boost:${module}")

        set(BOOST_RESOLVED ${BOOST_RESOLVED};${module} CACHE INTERNAL "Resolved Boost libraries" FORCE)

        if (NOT EXISTS "${ROOT}/libs/${module}/include")
            git_submodule("${ROOT}" "libs/${module}")
        endif()

        if (EXISTS "${ROOT}/libs/${module}/build/Jamfile.v2")
            set(BOOST_LIBRARIES ${BOOST_LIBRARIES} ${module} CACHE INTERNAL "Boost libraries" FORCE)
        else()
            set(BOOST_INCLUDES ${BOOST_INCLUDES} ${module} CACHE INTERNAL "Boost includes" FORCE)
        endif()

        set(MESSAGES_INDENTATION_TEMP ${MESSAGES_INDENTATION})
        set(MESSAGES_INDENTATION "${MESSAGES_INDENTATION}  " CACHE STRING "Messages indentation" FORCE)

        foreach(DEPENDENCY ${DEPENDENCIES_${module}})
            resolve(${DEPENDENCY})
        endforeach()

        set(MESSAGES_INDENTATION "${MESSAGES_INDENTATION_TEMP}" CACHE STRING "Messages indentation" FORCE)
    endif()
endfunction()

if(NOT "${SETUP_COMPONENTS}" STREQUAL "")
	set(OUTPUT_DIR ${OUTPUT_ROOT})
	set(LIBRARY_DIR ${OUTPUT_DIR}/lib)
	set(INCLUDE_DIR ${OUTPUT_DIR}/include/boost-1_67)

    foreach(COMPONENT ${SETUP_COMPONENTS})
        resolve("${COMPONENT}")
    endforeach()

	set(WITH)
    set(NEED_LIBRARIES FALSE)

	foreach(LIB ${BOOST_LIBRARIES})
		if (WIN32)
            file(GLOB FILE_PATH "${LIBRARY_DIR}/libboost_${LIB}*.lib")

            if("${FILE_PATH}" STREQUAL "")
                set(NEED_LIBRARIES TRUE)
				set(WITH ${WITH} "--with-${LIB}")
            else()
                message("${MESSAGES_INDENTATION}  > Found ${FILE_PATH}")
            endif()
		elseif(UNIX)
            if(NOT EXISTS "${LIBRARY_DIR}/libboost_${LIB}.a")
                set(NEED_LIBRARIES TRUE)
				set(WITH "${WITH},${LIB}")
            else()
                message("${MESSAGES_INDENTATION}  > Found ${LIBRARY_DIR}/libboost_${LIB}.a")
            endif()
		endif()
	endforeach()

	if(${SETUP_REINSTALL} OR ${NEED_LIBRARIES} OR NOT EXISTS "${INCLUDE_DIR}")
		if ("${MODULE_ARCH}" STREQUAL "x86")
			set(ADDRESS_MODEL 32)
		elseif ("${MODULE_ARCH}" STREQUAL "x64")
			set(ADDRESS_MODEL 64)
		endif()

		if(WIN32)
			set(BOOTSTRAP_COMMAND bootstrap.bat)
			set(B2_HEADERS ${ROOT}/tools/build/b2 headers --prefix=${OUTPUT_DIR})
			set(B2_COMMAND ${ROOT}/tools/build/b2 -j4 ${WITH} toolset=msvc -a define=BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE address-model=${ADDRESS_MODEL} variant=release,debug link=static threading=multi runtime-link=shared install --prefix=${OUTPUT_DIR})
		elseif(UNIX)
			set(BOOTSTRAP_COMMAND ${ROOT}/bootstrap.sh --with-libraries=${WITH} --prefix=${OUTPUT_DIR})
			set(B2_HEADERS ${ROOT}/b2 headers)
			set(B2_COMMAND ${ROOT}/b2 cxxflags=-fPIC cflags=-fPIC -a define=BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE address-model=${ADDRESS_MODEL} link=static threading=multi runtime-link=shared install)
        endif()

		if (UNIX AND NOT EXISTS ${ROOT}/b2)
        	message("${MESSAGES_INDENTATION}  - Configure boost...")
			execute_process(COMMAND ${BOOTSTRAP_COMMAND} WORKING_DIRECTORY ${ROOT}/tools/build RESULT_VARIABLE RESULT)

			if(NOT ${RESULT} EQUAL 0)
				message(FATAL_ERROR "Can't configure boost!")
			endif()
		endif ()

        message("${MESSAGES_INDENTATION}  - Install boost headers...")
        execute_process(COMMAND ${B2_HEADERS} WORKING_DIRECTORY ${ROOT} RESULT_VARIABLE RESULT)

		if(NOT ${RESULT} EQUAL 0)
            message(FATAL_ERROR "Can't install boost headers!")
        endif()

		if(${SETUP_REINSTALL} OR ${NEED_LIBRARIES})
			message("${MESSAGES_INDENTATION}  - Build boost libraries...")
			execute_process(COMMAND ${B2_COMMAND} WORKING_DIRECTORY ${ROOT} RESULT_VARIABLE RESULT)

			if(NOT ${RESULT} EQUAL 0)
				message(FATAL_ERROR "Can't build boost!")
			endif()

			if(WIN32)
				set(libs)

				foreach(COMPONENT ${BOOST_LIBRARIES})
					file(GLOB FILE_PATH ${LIBRARY_DIR}/libboost_${COMPONENT}*.lib)

					if("${FILE_PATH}" STREQUAL "")
						message(FATAL_ERROR "Can't install boost! ${COMPONENT} was not built")
					endif()

					set(libs ${libs} ${FILE_PATH})
				endforeach()
			elseif(UNIX)
				set(libs)

				foreach(COMPONENT ${BOOST_LIBRARIES})
					if(NOT EXISTS ${LIBRARY_DIR}/libboost_${COMPONENT}.a)
						message(FATAL_ERROR "Can't install boost!")
					endif()

					set(libs ${libs} ${LIBRARY_DIR}/libboost_${COMPONENT}.a)
				endforeach()
			endif()

			file(COPY ${libs} DESTINATION ${LIBRARY_OUTPUT})
		endif()
	endif()

	add_module_include_dirs("${INCLUDE_DIR}")

	target_compile_definitions(${PROJECT_NAME} PUBLIC "-DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE=1")
endif()
