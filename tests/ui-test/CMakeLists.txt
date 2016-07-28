#--------------------------------------------------------
#	Rapture UI Test
#--------------------------------------------------------

project(RaptureUITest CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION)
	dependencies(
		RaptureApplication "application"
		RaptureFreeImage   "freeimage"
		RaptureFreeType    "freetype"
	)

if(${USE_DIRECT3D} EQUAL 1)
    dependencies(RaptureDirect3D11 "direct3d")
else()
    dependencies(RaptureOpenGL3_3 "opengl")
endif()

	sources(tests)
		group(src Sources)
		files(ui-test.cpp)
	endsources()
endmodule()

load_third_party(vld)

#--------------------------------------------------------
