#--------------------------------------------------------
#	Rapture Direct3D Test
#--------------------------------------------------------

project(RaptureDirect3DTest CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION)
	dependencies(
		RaptureApplication "application"
		RaptureDirect3D11  "direct3d"
		RaptureFreeImage   "freeimage"
		RaptureFreeType    "freetype"
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