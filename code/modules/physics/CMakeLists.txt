#--------------------------------------------------------
#	asd Physics
#--------------------------------------------------------

project(physics VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(STATIC)
	dependencies(space	0.*)

	sources(modules)
		domain(physics)

		group(include Headers)
		files(
			Physics.h
			PhysicalLayer.h
			PlaneObject.h
		)

		group(src Sources)
		files(Physics.cpp)
	endsources()
endmodule()

vendor(bullet)

#--------------------------------------------------------
