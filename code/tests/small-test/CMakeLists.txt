#--------------------------------------------------------
#	Special small test
#--------------------------------------------------------

project(small_test VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION)
	dependencies(
		core	0.*
	)

	sources(tests)
		group(src Sources)
		files(small-test.cpp)
	endsources()
endmodule()

vendor(vld)

#--------------------------------------------------------
