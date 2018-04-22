#--------------------------------------------------------
#	module image
#--------------------------------------------------------

project(image VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(LIBRARY)
	dependencies(
		graphics	0.*
	)

	sources(modules)
		domain(image)

		group(include Headers)
		files(
			image.h
		)

		group(src Sources)
		files(
			image.cpp
		)
	endsources()
endmodule()

#--------------------------------------------------------
