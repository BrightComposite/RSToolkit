#--------------------------------------------------------
#	Rapture platform-dependent facilities
#--------------------------------------------------------

project(RapturePlatform CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(RaptureMath "math")

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