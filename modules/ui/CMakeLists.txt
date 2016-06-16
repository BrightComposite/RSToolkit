#--------------------------------------------------------
#	Rapture User Interface
#--------------------------------------------------------

project(RaptureUI CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(
	SHARED
    RaptureGraphics "graphics"
	RaptureApplication "application"
	RapturePlatform "platform"
)

set_module_folder(modules)

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION ui
			Panel.h
			UISpace.h
			Widget.h
			WidgetDefs.h
			WidgetMessages.h
			Window.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION ui
			Panel.cpp
			UISpace.cpp
			Widget.cpp
			Window.cpp
		END_SECTION
)

end_module()

#--------------------------------------------------------