#--------------------------------------------------------
#	module benchmark
#--------------------------------------------------------

project(benchmark VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(INLINE)
	dependencies(
		core	0.*
	)

	sources(modules)
		group(include Headers)
		files(
			benchmark.h
			benchmark
		)
	endsources()
endmodule()

#--------------------------------------------------------
