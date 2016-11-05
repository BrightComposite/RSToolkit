#--------------------------------------------------------
#	asd User Interface
#--------------------------------------------------------

project(asdUI CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(
		asdGraphics		"graphics"
		asdMessage		"message"
		asdApplication	"application"
		asdPlatform		"platform"
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