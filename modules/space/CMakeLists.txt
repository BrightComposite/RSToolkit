#--------------------------------------------------------
#	Rapture Space
#--------------------------------------------------------

project(RaptureSpace CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

module(INLINE)
	dependencies(
		RaptureMath "math"
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
