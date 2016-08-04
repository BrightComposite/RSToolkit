#--------------------------------------------------------
#	module @module_path@
#--------------------------------------------------------

project(@module_path@ CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(
		# RaptureCore "core"
	)

	sources("@module_domain@")
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