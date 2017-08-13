#--------------------------------------------------------
#	asd application
#--------------------------------------------------------

project(application VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(STATIC)
	dependencies(
		core		0.*
		platform	0.*
	)

	sources(modules)
		domain(application)

		group(include Headers)
		files(
			application.h
			starter.h
			thread_loop.h
			)

		group(src Sources)
		files(
			application.cpp
			thread_loop.cpp
			)
	endsources()
endmodule()