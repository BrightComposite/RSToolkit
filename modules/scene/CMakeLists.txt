#--------------------------------------------------------
#	Rapture Scene
#--------------------------------------------------------

project(RaptureScene CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

start_module()

require_modules(
	RaptureUI "ui"
)

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
	SRC_GROUP include Headers
		START_SECTION scene
			Scene.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION scene
			Scene.cpp
		END_SECTION
)

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})

end_module()

#--------------------------------------------------------