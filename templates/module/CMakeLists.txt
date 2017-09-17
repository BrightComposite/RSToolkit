#--------------------------------------------------------
#	module @module_path@
#--------------------------------------------------------

project(@module_path@ VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(STATIC)
	dependencies(
		# core	0.*
	)

	sources(@module_domain@)
		domain(@module_name@)
		
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