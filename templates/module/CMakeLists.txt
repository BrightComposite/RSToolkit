#--------------------------------------------------------
#	module @module_path@
#--------------------------------------------------------

project(@module_path@ CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(
		# asd-Core "core"
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