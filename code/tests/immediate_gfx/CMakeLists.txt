#--------------------------------------------------------
#	immediate_gfx test facility
#--------------------------------------------------------

project(immediate_gfx_test VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION CONSOLE)
	dependencies(
		application	0.*
		benchmark	0.*
	)

	sources(tests)
		group(src Sources)
		files(
			main.cpp
		)
	endsources()
endmodule()

if(WIN32)
	vendor(vld)
endif()

#--------------------------------------------------------
