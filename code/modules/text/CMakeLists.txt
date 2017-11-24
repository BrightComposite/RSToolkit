#--------------------------------------------------------
#	module text
#--------------------------------------------------------

project(text VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(LIBRARY)
	dependencies(
		graphics	0.*
		io			0.*
	)

	sources(modules)
		domain(text)
		
		group(include Headers)
		files(
			font.h
		)

		group(src Sources)
		files(
			font.cpp
		)
	endsources()
endmodule()

#--------------------------------------------------------
