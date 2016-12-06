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
			Cursor.h
			Text.h
			UIPalette.h
			UISpace.h
			UITheme.h
			Widget.h
			WidgetLayers.h
			WidgetEnum.h
			WidgetMessages.h
			Window.h
		)

		group(src Sources)
		files(
			Cursor.cpp
			Text.cpp
			UIPalette.cpp
			UISpace.cpp
			Widget.cpp
			WidgetLayers.cpp
			Window.cpp
		)
	endsources()
endmodule()

#--------------------------------------------------------