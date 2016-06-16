#--------------------------------------------------------
#	Rapture Basic Game
#--------------------------------------------------------

project(BasicGame CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(
	APPLICATION
    RaptureApplication "application"
    RapturePhysics     "physics"
	RaptureFreeImage   "freeimage"
	RaptureFreeType    "freetype"
)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    require_module(RaptureDirect3D11 "direct3d")
else()
    require_module(RaptureOpenGL3_3 "opengl")
endif()

set_module_api_key(basicgame)

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION game
			Game.h

			START_SECTION entity
				PlayerEntity.h
			END_SECTION

			START_SECTION object
				PlayerObject.h
			END_SECTION
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION game
			Game.cpp
		END_SECTION
)

end_module()

load_third_party(vld)

#--------------------------------------------------------
