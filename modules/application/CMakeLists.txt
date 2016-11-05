#--------------------------------------------------------
#	asd Application
#--------------------------------------------------------

project(asdApplication CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(
		asdCore "core"
		asdPlatform "platform"
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