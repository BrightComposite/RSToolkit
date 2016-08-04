#--------------------------------------------------------
#	Rapture OpenGL 3.3
#--------------------------------------------------------

project(RaptureOpenGL3_3 CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)
include(${RAPTURE_TOOLS}/shaders.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(RaptureUI "ui")
	api(opengl3_3)

	sources(modules)
		domain(opengl)
		
		group(include Headers)
		files(OpenGL3_3.h)

		group(src Sources)
		files(
			GLObjects.cpp
			GLSurfaces.cpp
			OpenGL3_3.cpp

			shaders/
				Shaders.cpp
		)

		domain(opengl/shaders)
		
		group(resources Resources)
		shaders(GLSL
			2d/
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
				instanced/
					cube/
						vs.glsl fs.glsl ..
				..
		)

		group(include Headers)
		domain(opengl/shaders)
		shaders_output(Shaders.h)
		
		domain(graphics)
		files(Provider.h)
	endsources()
endmodule()


load_third_party(opengl3_3)

#--------------------------------------------------------
