#--------------------------------------------------------
#	asd Interaction
#--------------------------------------------------------

project(interaction VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(STATIC)
	dependencies(input	0.*)

	sources(modules)
		domain(interaction)

		group(include Headers)
		files(mouse_manipulator.h)

		group(src Sources)
		files(mouse_manipulator.cpp)
	endsources()
endmodule()

#--------------------------------------------------------
