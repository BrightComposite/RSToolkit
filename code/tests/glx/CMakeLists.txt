#--------------------------------------------------------
#	glx test facility
#--------------------------------------------------------

project(glx_test VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION)
	dependencies(
		application	0.*
	)

	sources(tests)
		group(src Sources)
		files(
			main.cpp
		)
	endsources()
endmodule()

if(UNIX)
	find_package(X11 REQUIRED)
	
	if(NOT X11_FOUND)
		message(FATAL_ERROR "X11 not found!")
	endif()
	
	include_directories(${X11_INCLUDE_DIRS})
	link_directories(${X11_LIBRARY_DIRS})
	add_definitions(${X11_DEFINITIONS})
	
	target_link_libraries(${PROJECT_NAME} PRIVATE ${X11_LIBRARIES})
endif()

find_package(OpenGL REQUIRED)

if(NOT OPENGL_FOUND)
	message(FATAL_ERROR "OPENGL not found!")
endif()

include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})

#--------------------------------------------------------
