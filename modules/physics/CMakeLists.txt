#--------------------------------------------------------
#	asd Physics
#--------------------------------------------------------

project(asd-Physics CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(asd-Space "space")

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

load_third_party(bullet)

#--------------------------------------------------------