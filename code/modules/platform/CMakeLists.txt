#--------------------------------------------------------
#	asd platform-dependent facilities
#--------------------------------------------------------

project(platform VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(STATIC)
	dependencies(math	0.*)

	sources(modules)

	if(WIN32)
		domain(windows)

		group(include Headers)
		files(
			ComException.h
			ComHandle.h
			Display.h
			PointAdapter.h
			RectAdapter.h
			)

		group(src Sources)
		files(Display.cpp)
	elseif(UNIX)
		domain(unix)
			
		group(include Headers)
		files(Display.h)
			
		group(src Sources)
		files(Display.cpp)
	endif()

	endsources()
endmodule()

#--------------------------------------------------------