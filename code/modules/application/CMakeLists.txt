#--------------------------------------------------------
#	asd Application
#--------------------------------------------------------

project(application VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(
		core		0.*
		platform	0.*
	)

	sources(modules)
		domain(application)

		group(include Headers)
		files(
			Application.h
			Starter.h
			ThreadLoop.h
			)

		group(src Sources)
		files(
			Application.cpp
			ThreadLoop.cpp
			)
	endsources()
endmodule()

#--------------------------------------------------------