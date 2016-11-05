#--------------------------------------------------------
#	asd Space
#--------------------------------------------------------

project(asd-Space CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(INLINE)
	dependencies(
		asd-Math "math"
	)

	sources(modules)
		domain(space)

		group(include Headers)
		files(
			Spatial.h
		)
	endsources()
endmodule()

#--------------------------------------------------------
