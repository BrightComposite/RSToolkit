#--------------------------------------------------------
#	Rapture UI Test
#--------------------------------------------------------

project(RaptureUITest CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(
	APPLICATION
    RaptureApplication "application"
	RaptureFreeImage   "freeimage"
	RaptureFreeType    "freetype"
)

if(${USE_DIRECT3D} EQUAL 1)
    require_module(RaptureDirect3D11 "direct3d")
else()
    require_module(RaptureOpenGL3_3 "opengl")
endif()

set_module_folder(tests)

add_module_sources(
	SRC_GROUP src Sources
		START_SECTION SRC_ROOT
			ui-test.cpp
		END_SECTION
)

end_module()

load_third_party(vld)

#--------------------------------------------------------
