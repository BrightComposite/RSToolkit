#--------------------------------------------------------
#	space test facility
#--------------------------------------------------------

project(space_test VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION CONSOLE)
	dependencies(
		application	0.*
		space		0.*
	)

	sources(tests)
		group(src Sources)
		files(
			main.cpp
		)
	endsources()
endmodule()

if(${WIN32})
	# vendor(vld)
endif()

#--------------------------------------------------------
