#--------------------------------------------------------
#	asd FreeImage adaptation
#--------------------------------------------------------

project(freeimage4asd VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(graphics	0.*)

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