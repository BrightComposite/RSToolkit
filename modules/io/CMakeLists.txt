#--------------------------------------------------------
#	asd Input/Output facilities
#--------------------------------------------------------

project(io VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(INLINE)
	dependencies(meta	0.*)

	sources(modules)
		domain(io)
		group(include Headers)
		files(FileSystem.h)
	endsources()
endmodule()

setup_third_party(boost COMPONENT filesystem)
setup_third_party(boost COMPONENT system)

#--------------------------------------------------------