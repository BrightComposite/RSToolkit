#--------------------------------------------------------
#	asd Direct3D Test
#--------------------------------------------------------

project(asd-Direct3DTest CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION)
	dependencies(
		asd-Application "application"
		asd-Direct3D11  "direct3d"
		asd-FreeImage   "freeimage"
		asd-FreeType    "freetype"
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

load_third_party(vld)

#--------------------------------------------------------