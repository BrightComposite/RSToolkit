#--------------------------------------------------------
#	Rapture Application
#--------------------------------------------------------

project(RaptureApplication CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(
		RaptureCore "core"
		RapturePlatform "platform"
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