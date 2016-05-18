#--------------------------------------------------------
#	Rapture Input/Output facilities
#--------------------------------------------------------

project(RaptureIO CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

start_module()

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
	SRC_GROUP include Headers
		START_SECTION io
			FileSystem.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION io
			io.cpp
		END_SECTION
)

#--------------------------------------------------------

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
setup_third_party(boost COMPONENT filesystem)
setup_third_party(boost COMPONENT system)

end_module()

#--------------------------------------------------------