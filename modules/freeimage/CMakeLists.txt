#--------------------------------------------------------
#	Rapture FreeImage adaptation
#--------------------------------------------------------

project(RaptureFreeImage CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(
	SHARED
	RaptureGraphics "graphics"
)

set_module_folder(modules)
set_module_api_key(freeimage)

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION freeimage
			FreeImageConverter.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION freeimage
			FreeImageConverter.cpp
		END_SECTION
)

end_module()

load_third_party(freeimage)

#--------------------------------------------------------