#--------------------------------------------------------
#	Rapture Interaction
#--------------------------------------------------------

project(RaptureInteraction CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(RaptureUI "ui")

	sources(modules)
		domain(interaction)
		
		group(include Headers)
		files(MouseManipulator.h)

		group(src Sources)
		files(MouseManipulator.cpp)
	endsources()
endmodule()

#--------------------------------------------------------
