#--------------------------------------------------------
#	Rapture FreeImage adaptation
#--------------------------------------------------------

project(RaptureFreeImage CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

start_module()

require_modules(
	RaptureGraphics "graphics"
)

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
	SRC_GROUP include Headers
		START_SECTION freeimage
			FreeImageConverter.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION freeimage
			FreeImageConverter.cpp
		END_SECTION
)

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
load_third_party(freeimage)

end_module()

#--------------------------------------------------------