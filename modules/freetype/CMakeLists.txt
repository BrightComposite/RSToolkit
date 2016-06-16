#--------------------------------------------------------
#	Rapture FreeType adaptation
#--------------------------------------------------------

project(RaptureFreeType CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(
	SHARED
	RaptureGraphics "graphics"
)

set_module_api_key(freetype)

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION freetype
			FreeTypeDecoder.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION freetype
			FreeTypeDecoder.cpp
		END_SECTION
)

end_module()

load_third_party(freetype)

#--------------------------------------------------------