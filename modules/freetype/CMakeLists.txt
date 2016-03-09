#--------------------------------------------------------
#	Rapture FreeType adaptation
#--------------------------------------------------------

project(RaptureFreeType CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

require_modules(
	RaptureGraphics "graphics"
)

start_module()

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
	SRC_GROUP include Headers
		START_SECTION freetype
			FreeTypeDecoder.h
		END_SECTION
	
	SRC_GROUP src Sources
		START_SECTION freetype
			FreeTypeDecoder.cpp
		END_SECTION
)

#--------------------------------------------------------
		
add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

#--------------------------------------------------------

set_project_include_dirs(
	${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC RaptureGraphics)

#--------------------------------------------------------

target_load_third_party(${PROJECT_NAME} freetype)

#--------------------------------------------------------