#--------------------------------------------------------
#	asd Core Test
#--------------------------------------------------------

project(coretest VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION)
	dependencies(
		application 0.*
		message 	0.*
	)

	sources(tests)
		group(src Sources)
		files(core-test.cpp)
	endsources()
endmodule()

load_third_party(vld)

#--------------------------------------------------------