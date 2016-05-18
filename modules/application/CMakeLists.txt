#--------------------------------------------------------
#	Rapture Application
#--------------------------------------------------------

project(RaptureApplication CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

start_module()

require_modules(
	RaptureCore "core"
)

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
	SRC_GROUP include Headers
		START_SECTION application
			Application.h
			ThreadLoop.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION application
			Application.cpp
			ThreadLoop.cpp
		END_SECTION
)

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})

end_module()

#--------------------------------------------------------