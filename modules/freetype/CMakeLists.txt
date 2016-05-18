#--------------------------------------------------------
#	Rapture FreeType adaptation
#--------------------------------------------------------

project(RaptureFreeType CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

start_module()

require_modules(
	RaptureGraphics "graphics"
)

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

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
load_third_party(freetype)

end_module()

#--------------------------------------------------------