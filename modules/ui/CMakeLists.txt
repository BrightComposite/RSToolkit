#--------------------------------------------------------
#	Rapture User Interface
#--------------------------------------------------------

project(RaptureUI CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

require_modules(
    RaptureGraphics "graphics"
	RaptureApplication "application"
	RapturePlatform "platform"
)

start_module()

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
	SRC_GROUP include Headers
		START_SECTION ui
			Panel.h
			Widget.h
			WidgetDefs.h
			WidgetMessages.h
			Window.h
			WindowAdapter.h
		END_SECTION
			
	SRC_GROUP src Sources
		START_SECTION ui
			Panel.cpp
			Widget.cpp
			Window.cpp
			WindowAdapter.cpp
		END_SECTION
)

#--------------------------------------------------------

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

#--------------------------------------------------------

set_project_include_dirs(
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/inline-src
)

target_link_libraries(${PROJECT_NAME} PUBLIC RaptureGraphics RaptureApplication RapturePlatform)

#--------------------------------------------------------