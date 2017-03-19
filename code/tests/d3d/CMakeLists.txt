#--------------------------------------------------------
#	Direct3D Test
#--------------------------------------------------------

project(direct3dtest VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION)
	dependencies(
		application	0.*
		direct3d	0.*
		freeimage	0.*
		freetype	0.*
	)

	sources(tests)
		group(include Headers)
		files(matrix-test.h)

		group(src Sources)
		files(
			d3d-test.cpp
			second.cpp
		)
	endsources()
endmodule()

vendor(vld)

#--------------------------------------------------------