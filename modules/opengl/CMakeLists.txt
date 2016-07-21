#--------------------------------------------------------
#	Rapture OpenGL 3.3
#--------------------------------------------------------

project(RaptureOpenGL3_3 CXX)

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

set_module_folder(modules)
set_module_api_key(opengl3_3)

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION graphics
			Provider.h
		END_SECTION

		START_SECTION opengl
			OpenGL3_3.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION opengl
			GLObjects.cpp
			GLSurfaces.cpp
			OpenGL3_3.cpp

			START_SECTION shaders
				Shaders.cpp
			END_SECTION
		END_SECTION
)

add_shaders(
	GLSL
	resources Resources include Includes
	opengl/shaders
	Shaders.h
		START_SECTION 2d
			START_SECTION rect
				vs.glsl
				fs.glsl
			END_SECTION

			START_SECTION ellipse
				vs.glsl
				fs.glsl
			END_SECTION

			START_SECTION figure
				vs.glsl
				fs.glsl
			END_SECTION

			START_SECTION image
				vs.glsl
				fs.glsl
			END_SECTION

			START_SECTION text
				vs.glsl
				fs.glsl
			END_SECTION

			START_SECTION wired
				START_SECTION rect
					vs.glsl
					fs.glsl
				END_SECTION

				START_SECTION ellipse
					vs.glsl
					fs.glsl
				END_SECTION
			END_SECTION
		END_SECTION

		START_SECTION 3d
			START_SECTION basic
				vs.glsl
				fs.glsl
			END_SECTION

			START_SECTION color
				vs.glsl
				fs.glsl
			END_SECTION

			START_SECTION multicolor
				vs.glsl
				fs.glsl
			END_SECTION

			START_SECTION texture
				vs.glsl
				fs.glsl
			END_SECTION
		END_SECTION
)

end_module()

load_third_party(opengl3_3)

#--------------------------------------------------------
