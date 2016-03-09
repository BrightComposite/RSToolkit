#--------------------------------------------------------
#	Rapture DevIL adaptation
#--------------------------------------------------------

project(RaptureDevIL CXX)

#--------------------------------------------------------

include(${RAPTURE_ROOT}/tools/project.cmake)

#--------------------------------------------------------

set_dependencies(
	RaptureImageIO
)

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
	SRC_GROUP include Headers
		START_SECTION devil
			DevILImageConverter.h
		END_SECTION
	
	SRC_GROUP src Sources
		START_SECTION devil
			DevILImageConverter.cpp
		END_SECTION
)

#--------------------------------------------------------
		
add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

#--------------------------------------------------------

set_project_include_dirs(
	${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC RaptureImageIO)

#--------------------------------------------------------

target_load_third_party(${PROJECT_NAME} devil)
target_load_third_party(${PROJECT_NAME} zlib)

#--------------------------------------------------------