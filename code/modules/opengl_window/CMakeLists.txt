#--------------------------------------------------------
#	module opengl_window
#--------------------------------------------------------

project(opengl_window VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(LIBRARY)
	dependencies(
		opengl 	0.*
		window	0.*
	)

	sources(modules)
		domain(opengl_window)

		group(include Headers)
		files(
			context.h
		)

		group(src Sources)

		if (WIN32)
			files(windows/context.cpp)
		elseif(UNIX)
			files(linux/context.cpp)
		else()
			message(FATAL_ERROR "OpenGL integration is not implemented for this platform yet")
		endif()

	endsources()
endmodule()

#--------------------------------------------------------
