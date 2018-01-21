#--------------------------------------------------------
#	module sdlpp
#--------------------------------------------------------

project(sdlpp VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(LIBRARY)
	dependencies(
		# core	0.*
	)

	sources(modules)
		domain(sdlpp)
		
		group(include Headers)
		files(
			# headers
		)

		group(src Sources)
		files(
			# sources
		)
	endsources()
endmodule()

vendor(sdl2)

#--------------------------------------------------------
