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
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

#--------------------------------------------------------

set_project_include_dirs(
	${PROJECT_SOURCE_DIR}/include
	${BOOST_SOURCE_DIR}/include
)

#--------------------------------------------------------

target_setup_third_party(${PROJECT_NAME} boost COMPONENT filesystem)
target_setup_third_party(${PROJECT_NAME} boost COMPONENT system)

#--------------------------------------------------------