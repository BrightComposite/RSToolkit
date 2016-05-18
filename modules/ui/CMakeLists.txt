#--------------------------------------------------------
#	Rapture User Interface
#--------------------------------------------------------

project(RaptureUI CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

start_module()

require_modules(
    RaptureGraphics "graphics"
	RaptureApplication "application"
	RapturePlatform "platform"
)

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
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

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})

end_module()

#--------------------------------------------------------