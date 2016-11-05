#--------------------------------------------------------
#	asd Small Test
#--------------------------------------------------------

project(asd-SmallTest CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION)
	dependencies(asd-Core "core")

	sources(tests)
		group(src Sources)
		files(small-test.cpp)
	endsources()
endmodule()

load_third_party(vld)

#--------------------------------------------------------
