#--------------------------------------------------------
#	asd Input/Output facilities
#--------------------------------------------------------

project(io VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(STATIC)
	dependencies(core	0.*)

	sources(modules)
		domain(io)
		group(include Headers)
		files(filesystem.h)
		group(src sources)
		files(filesystem.cpp)
	endsources()
endmodule()

vendor(boost COMPONENTS filesystem)

#--------------------------------------------------------
