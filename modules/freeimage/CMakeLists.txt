#--------------------------------------------------------
#	asd FreeImage adaptation
#--------------------------------------------------------

project(asdFreeImage CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(asdGraphics "graphics")

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