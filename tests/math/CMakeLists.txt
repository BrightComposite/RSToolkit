#--------------------------------------------------------
#	asd Math test facility
#--------------------------------------------------------

project(mathtest VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION)
	dependencies(
		application	0.*
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
