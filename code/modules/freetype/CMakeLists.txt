#--------------------------------------------------------
#	asd FreeType adaptation
#--------------------------------------------------------

project(freetype-asd VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(STATIC)
	dependencies(
		text		0.*
	)
	api(freetype)

	sources(modules)
		domain(freetype)

		group(include Headers)
		files(freetype_component.h)

		group(src Sources)
		files(freetype_component.cpp)
	endsources()
endmodule()

vendor(freetype)

#--------------------------------------------------------
