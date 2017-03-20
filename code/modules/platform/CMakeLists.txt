#--------------------------------------------------------
#	asd platform-dependent facilities
#--------------------------------------------------------

project(platform VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(math	0.*)

	sources(modules)

	if(WIN32)
		domain(windows)

		group(include Headers)
		files(
			ComException.h
			ComHandle.h
			Message.h
			PointAdapter.h
			RectAdapter.h
			)

		group(src Sources)
		files(Message.cpp)
	elseif(UNIX)
		domain(unix)
			
		group(include Headers)
		files(Platform.h)
			
		group(src Sources)
		files(Platform.cpp)
	endif()

	endsources()
endmodule()

#--------------------------------------------------------
