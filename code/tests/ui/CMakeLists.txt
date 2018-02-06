#--------------------------------------------------------
#	asd UI Test
#--------------------------------------------------------

project(ui_test VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION CONSOLE)
	dependencies(
		application	0.*
		freetype	0.*
		window		0.*
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

if(${WIN32})
	# vendor(vld)
endif()

#--------------------------------------------------------
