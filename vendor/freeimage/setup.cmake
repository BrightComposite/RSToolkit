#--------------------------------------------------------
#	vendor setup file
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

if(WIN32)
	add_subdirectory(${CMAKE_CURRENT_LIST_DIR} ${OUTPUT_ROOT}/vendor/freeimage)
	add_dependencies(${PROJECT_NAME} freeimage)
elseif(UNIX)
	if(NOT EXISTS ${LIBRARY_OUTPUT}/libfreeimage.a OR NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/FreeImage/Dist)
		if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/FreeImage/Dist)
			execute_process(COMMAND make clean WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/FreeImage RESULT_VARIABLE RESULT)
			execute_process(COMMAND make WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/FreeImage RESULT_VARIABLE RESULT)
			
			if(NOT ${RESULT} EQUAL 0)
				message(FATAL_ERROR "Can't make freeimage!")
			endif()
		endif()
		
		file(GLOB libs ${CMAKE_CURRENT_LIST_DIR}/FreeImage/Dist/*.a ${CMAKE_CURRENT_LIST_DIR}/FreeImage/Dist/*.so)
		file(COPY ${libs} DESTINATION ${LIBRARY_OUTPUT})
	endif()

	include_directories(${CMAKE_CURRENT_LIST_DIR}/FreeImage/Dist)
	target_link_libraries(${PROJECT_NAME} PUBLIC freeimage)
else()
	message(FATAL_ERROR "Unsupported platform! Needed to implement")
endif()