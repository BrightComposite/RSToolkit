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

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    require_module(RaptureDirect3D11 "direct3d")
else()
    require_module(RaptureOpenGL3_3 "opengl")
endif()

add_module_sources(
	SRC_GROUP src Sources
		START_SECTION SRC_ROOT
			ui-test.cpp
		END_SECTION
)

end_module()

load_third_party(vld)

#--------------------------------------------------------
