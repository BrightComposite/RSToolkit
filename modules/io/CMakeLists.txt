#--------------------------------------------------------
#	asd Input/Output facilities
#--------------------------------------------------------

project(asdIO CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(INLINE)
	dependencies(asdMeta "meta")

	sources(modules)
		domain(io)
		group(include Headers)
		files(FileSystem.h)
	endsources()
endmodule()

setup_third_party(boost COMPONENT filesystem)
setup_third_party(boost COMPONENT system)

#--------------------------------------------------------