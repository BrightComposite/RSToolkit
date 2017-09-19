#--------------------------------------------------------
#	asd OpenGL
#--------------------------------------------------------

project(opengl VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

include(tools/shaders.cmake)

#--------------------------------------------------------

module(STATIC)
	dependencies(
		graphics	0.*
		window		0.*
	)

	api(opengl)

	sources(modules)
		domain(opengl)

		group(include Headers)
		files(
			mesh.h
			opengl.h
			vertex_layout.h
		)

		group(src Sources)
		files(
			mesh.cpp
			opengl.cpp
			vertex_layout.cpp
		)

		domain(opengl/shaders)

		group(resources Resources)
		shaders(
			2d/
                color/
                    vs.glsl fs.glsl ..
				rect/
					vs.glsl fs.glsl ..
				ellipse/
					vs.glsl fs.glsl ..
				figure/
					vs.glsl fs.glsl ..
				image/
					vs.glsl fs.glsl ..
				text/
					vs.glsl fs.glsl ..
				wired/
					rect/
						vs.glsl fs.glsl ..
					ellipse/
						vs.glsl fs.glsl
			.
			3d/
				basic/
					vs.glsl fs.glsl ..
				color/
					vs.glsl fs.glsl ..
				multicolor/
					vs.glsl fs.glsl ..
				texture/
					vs.glsl fs.glsl ..
		)

		group(include Headers)
		domain(opengl/shaders)
		shaders_output(shaders.h)
	endsources()
endmodule()

vendor(opengl)

#--------------------------------------------------------
