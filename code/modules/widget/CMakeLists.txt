#--------------------------------------------------------
#	module widget
#--------------------------------------------------------

project(widget VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(LIBRARY)
	dependencies(
		# core	0.*
	)

	sources(modules)
		domain(widget)
		
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

#--------------------------------------------------------
