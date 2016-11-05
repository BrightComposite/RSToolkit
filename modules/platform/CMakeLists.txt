#--------------------------------------------------------
#	asd platform-dependent facilities
#--------------------------------------------------------

project(asdPlatform CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(asdMath "math")

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

	endif()

	endsources()
endmodule()

#--------------------------------------------------------