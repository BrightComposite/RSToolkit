#--------------------------------------------------------
#	Rapture Direct3D11
#--------------------------------------------------------

project(RaptureDirect3D11 CXX)

#--------------------------------------------------------

set(RAPTURE_TOOLS ${RAPTURE_ROOT}/tools)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)
include(${RAPTURE_TOOLS}/shaders.cmake)

#--------------------------------------------------------

start_module(
	SHARED
	RaptureUI "ui"
)

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION direct3d
			Direct3D11.h
		END_SECTION

		START_SECTION graphics
			Provider.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION direct3d
			Direct3D11.cpp

			START_SECTION shaders
				Shaders.cpp
			END_SECTION
		END_SECTION
)

add_shaders(
	resources Resources
	include Includes
	direct3d/shaders
		START_SECTION 2d
			START_SECTION rect
				vs.fx
				ps.fx
			END_SECTION

			START_SECTION ellipse
				vs.fx
				ps.fx
			END_SECTION

			START_SECTION figure
				vs.fx
				ps.fx
			END_SECTION

			START_SECTION image
				vs.fx
				ps.fx
			END_SECTION

			START_SECTION text
				vs.fx
				ps.fx
			END_SECTION

			START_SECTION wired
				START_SECTION rect
					vs.fx
					ps.fx
				END_SECTION

				START_SECTION ellipse
					vs.fx
					ps.fx
				END_SECTION
			END_SECTION
		END_SECTION

		START_SECTION 3d
			START_SECTION basic
				vs.fx
				ps.fx
			END_SECTION

			START_SECTION color
				vs.fx
				ps.fx
			END_SECTION

			START_SECTION multicolor
				vs.fx
				ps.fx
			END_SECTION

			START_SECTION texture
				vs.fx
				ps.fx
			END_SECTION
		END_SECTION
)

end_module()

load_third_party(direct3d11)

#--------------------------------------------------------