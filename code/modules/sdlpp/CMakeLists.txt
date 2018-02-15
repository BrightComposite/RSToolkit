#--------------------------------------------------------
#	module sdlpp
#--------------------------------------------------------

project(sdlpp VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(LIBRARY)
	dependencies(
		opengl 		0.*
	)

	sources(modules)
		domain(sdlpp)

		group(include Headers)
		files(
			sdlpp.h
		)

		group(src Sources)
		files(
			sdlpp.cpp
		)
	endsources()
endmodule()

vendor(sdl2)

#--------------------------------------------------------
