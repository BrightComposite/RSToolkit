#--------------------------------------------------------
#	Rapture Core Test
#--------------------------------------------------------

project(RaptureCoreTest CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION)
	dependencies(
		RaptureApplication "application"
		RaptureMessage "message"
	)

	sources(tests)
		group(src Sources)
		files(core-test.cpp)
	endsources()
endmodule()

load_third_party(vld)

#--------------------------------------------------------