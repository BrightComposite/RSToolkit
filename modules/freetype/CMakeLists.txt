#--------------------------------------------------------
#	asd FreeType adaptation
#--------------------------------------------------------

project(asdFreeType CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(asdGraphics "graphics")
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