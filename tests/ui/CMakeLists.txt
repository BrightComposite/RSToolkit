#--------------------------------------------------------
#	asd UI Test
#--------------------------------------------------------

project(uitest VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION)
	dependencies(
		application	0.*
		freeimage	0.*
		freetype	0.*
		ui			0.*
	)

if(${USE_DIRECT3D} EQUAL 1)
    dependencies(direct3d 	0.*)
else()
    dependencies(opengl 	0.*)
endif()

	sources(tests)
		group(src Sources)
		files(ui-test.cpp)
	endsources()
endmodule()

load_third_party(vld)

#--------------------------------------------------------
