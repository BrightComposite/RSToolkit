#--------------------------------------------------------
#	Rapture FreeImage adaptation
#--------------------------------------------------------

project(RaptureFreeImage CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(RaptureGraphics "graphics")

	api(freeimage)
	
	sources(modules)
		domain(freeimage)
		
		group(include Headers)
		files(FreeImageConverter.h)

		group(src Sources)
		files(FreeImageConverter.cpp)
	endsources()
endmodule()

load_third_party(freeimage)

#--------------------------------------------------------