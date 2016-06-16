#--------------------------------------------------------
#	Rapture Direct3D Test
#--------------------------------------------------------

project(RaptureDirect3DTest CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(
	APPLICATION
    RaptureApplication "application"
	RaptureDirect3D11  "direct3d"
	RaptureFreeImage   "freeimage"
	RaptureFreeType    "freetype"
)

set_module_folder(tests)

add_module_sources(
	SRC_GROUP src Sources
		START_SECTION SRC_ROOT
			matrix-test.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION SRC_ROOT
			d3d-test.cpp
			second.cpp
		END_SECTION
)

end_module()

load_third_party(vld)

#--------------------------------------------------------