#--------------------------------------------------------
#	module input
#--------------------------------------------------------

project(input VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(INLINE)
	dependencies(
		core	0.*
	)

	sources(modules)
		domain(input)
		
		group(include Headers)
		files(
			keyboard.h
			mouse.h
		)

		group(src Sources)
		files(
			# sources
		)
	endsources()
endmodule()

#--------------------------------------------------------
