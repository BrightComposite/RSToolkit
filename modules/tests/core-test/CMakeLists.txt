#--------------------------------------------------------
#	Rapture Core Test
#--------------------------------------------------------

project(RaptureCoreTest CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

start_module()

require_modules(
    RaptureApplication "application"
)

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
	SRC_GROUP src Sources
		START_SECTION SRC_ROOT
			core-test.cpp
		END_SECTION
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
load_third_party(vld)

end_module()

#--------------------------------------------------------