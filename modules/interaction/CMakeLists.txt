#--------------------------------------------------------
#	asd Interaction
#--------------------------------------------------------

project(interaction VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(ui	0.*)

	sources(modules)
		domain(interaction)

		group(include Headers)
		files(MouseManipulator.h)

		group(src Sources)
		files(MouseManipulator.cpp)
	endsources()
endmodule()

#--------------------------------------------------------
