#--------------------------------------------------------
#	asd UI Test
#--------------------------------------------------------

project(asdUITest CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION)
	dependencies(
		asdApplication "application"
		asdFreeImage   "freeimage"
		asdFreeType    "freetype"
	)

if(${USE_DIRECT3D} EQUAL 1)
    dependencies(asdDirect3D11 "direct3d")
else()
    dependencies(asdOpenGL3_3 "opengl")
endif()

	sources(tests)
		group(src Sources)
		files(ui-test.cpp)
	endsources()
endmodule()

load_third_party(vld)

#--------------------------------------------------------
