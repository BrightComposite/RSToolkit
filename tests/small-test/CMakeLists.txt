#--------------------------------------------------------
#	Rapture Small Test
#--------------------------------------------------------

project(RaptureSmallTest CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION)
	dependencies(RaptureCore "core")

	sources(tests)
		group(src Sources)
		files(small-test.cpp)
	endsources()
endmodule()

load_third_party(vld)

#--------------------------------------------------------
