#--------------------------------------------------------
#	asd User Interface
#--------------------------------------------------------

project(ui VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(
		graphics	0.*
		message		0.*
		application	0.*
		platform	0.*
	)

	sources(modules)
		domain(ui)

		group(include Headers)
		files(
			Text.h
			UIPalette.h
			UISpace.h
			UITheme.h
			Widget.h
			WidgetLayers.h
			WidgetEnum.h
			WidgetMessages.h
			Window.h
			WindowEnum.h
		)

		group(src Sources)
		files(
			Text.cpp
			UIPalette.cpp
			UISpace.cpp
			Widget.cpp
			WidgetLayers.cpp
		)

		if(${WIN32})
			domain(ui/windows)
			group(include Headers)
			files(Cursor.h)
				
			group(src Sources)
			files(Cursor.cpp)
		else(${UNIX})
			domain(ui/x)
		endif()

		group(include Headers)
		files(Window.h)
		
		group(src Sources)
		files(Window.cpp)
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