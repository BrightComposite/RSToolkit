#--------------------------------------------------------
#	core test
#--------------------------------------------------------

project(coretest VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION CONSOLE)
	dependencies(
		application 0.*
		message 	0.*
	)

	sources(tests)
		group(src Sources)
		files(core-test.cpp)
	endsources()
endmodule()

if(${WIN32})
	vendor(vld)
endif()

#--------------------------------------------------------