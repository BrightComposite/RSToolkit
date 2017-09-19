#--------------------------------------------------------
#	asd graphics
#--------------------------------------------------------

project(graphics VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(LIBRARY)
	dependencies(
		math		0.*
		platform	0.*
	)

	sources(modules)
		domain(graphics)

		group(include Headers)
		files(
			graphics.h
			)

		group(src Sources)
		files(
			graphics.cpp
			)
	endsources()
endmodule()

#--------------------------------------------------------