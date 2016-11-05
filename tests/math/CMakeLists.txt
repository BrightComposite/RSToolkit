#--------------------------------------------------------
#	asdMath test facility
#--------------------------------------------------------

project(asdMathTest CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION)
	dependencies(
		asdApplication "application"
	)

	sources(tests)
		group(src Sources)
		files(
			main.cpp
		)
	endsources()
endmodule()

load_third_party(vld)

#--------------------------------------------------------
