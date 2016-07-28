#--------------------------------------------------------
#	Rapture Input/Output facilities
#--------------------------------------------------------

project(RaptureIO CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

module(INLINE)
	dependencies(RaptureMeta "meta")

	sources(modules)
		domain(io)
		group(include Headers)
		files(FileSystem.h)
	endsources()
endmodule()

setup_third_party(boost COMPONENT filesystem)
setup_third_party(boost COMPONENT system)

#--------------------------------------------------------