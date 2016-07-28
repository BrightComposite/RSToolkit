#--------------------------------------------------------
#	Rapture FreeType adaptation
#--------------------------------------------------------

project(RaptureFreeType CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(RaptureGraphics "graphics")
	api(freetype)
	
	sources(modules)
		domain(freetype)
		
		group(include Headers)
		files(FreeTypeDecoder.h)

		group(src Sources)
		files(FreeTypeDecoder.cpp)
	endsources()
endmodule()

load_third_party(freetype)

#--------------------------------------------------------