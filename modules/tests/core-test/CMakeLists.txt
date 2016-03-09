#--------------------------------------------------------
#	Rapture Core Test
#--------------------------------------------------------

project(RaptureCoreTest CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

require_modules(
    RaptureApplication "application"
)

start_module()

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
	SRC_GROUP src Sources
		START_SECTION SRC_ROOT
			core-test.cpp
		END_SECTION
)

#--------------------------------------------------------
			
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

#--------------------------------------------------------

set_project_include_dirs(
	${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC RaptureApplication)

#--------------------------------------------------------

target_load_third_party(${PROJECT_NAME} vld)

#--------------------------------------------------------