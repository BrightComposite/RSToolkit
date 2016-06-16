#--------------------------------------------------------
#	Independence
#--------------------------------------------------------

project(Independence CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(
	APPLICATION
    RaptureApplication "application"
	RaptureScene       "scene"
	RaptureFreeImage   "freeimage"
	RaptureFreeType    "freetype"
)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    require_module(RaptureDirect3D11 "direct3d")
else()
    require_module(RaptureOpenGL3_3 "opengl")
endif()

set_module_folder(projects)

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION art
			START_SECTION object
				Snake.h
			END_SECTION
		END_SECTION

		START_SECTION utils
			FpsCounter.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION SRC_ROOT
			main.cpp
		END_SECTION
)

end_module()

load_third_party(vld)

#--------------------------------------------------------