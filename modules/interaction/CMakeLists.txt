#--------------------------------------------------------
#	asd Interaction
#--------------------------------------------------------

project(asd-Interaction CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(asd-UI "ui")

	sources(modules)
		domain(interaction)

		group(include Headers)
		files(MouseManipulator.h)

		group(src Sources)
		files(MouseManipulator.cpp)
	endsources()
endmodule()

#--------------------------------------------------------
