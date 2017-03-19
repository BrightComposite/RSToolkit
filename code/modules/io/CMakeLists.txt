#--------------------------------------------------------
#	asd Input/Output facilities
#--------------------------------------------------------

project(io VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(meta	0.*)

	sources(modules)
		domain(io)
		group(include Headers)
		files(FileSystem.h)
		group(src Sources)
		files(FileSystem.cpp)
	endsources()
endmodule()

vendor(boost LIBRARIES system filesystem)

#--------------------------------------------------------