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
    RaptureMessage "message"
	RaptureApplication "application"
	RapturePlatform "platform"
)

set_module_folder(modules)

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION ui
			Cursor.h
			Panel.h
			Text.h
			UISpace.h
			Widget.h
			WidgetEnum.h
			WidgetMessages.h
			Window.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION ui
			Cursor.cpp
			Panel.cpp
			Text.cpp
			UISpace.cpp
			Widget.cpp
			Window.cpp
		END_SECTION
)

end_module()

#--------------------------------------------------------