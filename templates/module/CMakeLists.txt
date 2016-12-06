#--------------------------------------------------------
#	module @module_path@
#--------------------------------------------------------

project(@module_path@ VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(
		# core	0.*
	)

	sources(@module_domain@)
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