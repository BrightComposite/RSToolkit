#--------------------------------------------------------
#	module flow
#--------------------------------------------------------

project(flow VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(INLINE)
	dependencies(
		core	0.*
	)

	sources(modules)
		domain(flow)

		group(include Headers)
		files(
			context.h
			timer.h
		)

		group(src Sources)
		files(
			# sources
		)
	endsources()
endmodule()

vendor(boost COMPONENTS asio)

#--------------------------------------------------------
