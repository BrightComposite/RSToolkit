#--------------------------------------------------------
#	Rapture User Interface
#--------------------------------------------------------

project(RaptureUI CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(
		RaptureGraphics		"graphics"
		RaptureMessage		"message"
		RaptureApplication	"application"
		RapturePlatform		"platform"
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