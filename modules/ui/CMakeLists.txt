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
			Panel.h
			Text.h
			UISpace.h
			Widget.h
			WidgetEnum.h
			WidgetMessages.h
			Window.h
		)

		group(src Sources)
		files(
			Cursor.cpp
			Panel.cpp
			Text.cpp
			UISpace.cpp
			Widget.cpp
			Window.cpp
		)
	endsources()
endmodule()

#--------------------------------------------------------