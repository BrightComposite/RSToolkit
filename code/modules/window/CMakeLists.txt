#--------------------------------------------------------
#	module window
#--------------------------------------------------------

project(window VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(LIBRARY)
	dependencies(
		graphics	0.*
	)

	sources(modules)
		domain(window)
		
		group(include Headers)
		files(
			window.h
		)

		group(src Sources)
		files(
			window.cpp
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

vendor(observable GIT https://github.com/ddinu/observable)

#--------------------------------------------------------
