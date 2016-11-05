#--------------------------------------------------------
#	asd Core Test
#--------------------------------------------------------

project(asdCoreTest CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION)
	dependencies(
		asdApplication "application"
		asdMessage "message"
	)

	sources(tests)
		group(src Sources)
		files(core-test.cpp)
	endsources()
endmodule()

load_third_party(vld)

#--------------------------------------------------------