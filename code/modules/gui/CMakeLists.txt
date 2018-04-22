#--------------------------------------------------------
#	asd Graphic User Interface
#--------------------------------------------------------

project(gui VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(STATIC)
	dependencies(
		graphics	0.*
		application	0.*
	)

	sources(modules)
		domain(gui)

		group(include Headers)
		files(
			space.h
			object.h
			layer.h
			mouse_buttons.h
		)

		group(src Sources)
		files(
			space.cpp
			object.cpp
			layer.cpp
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

#--------------------------------------------------------
