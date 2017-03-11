#--------------------------------------------------------
#	Special small test
#--------------------------------------------------------

project(smalltest VERSION 0.1)

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

load_third_party(vld)

#--------------------------------------------------------
