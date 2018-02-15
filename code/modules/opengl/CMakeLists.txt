#--------------------------------------------------------
#	asd OpenGL
#--------------------------------------------------------

project(opengl VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

include(tools/shaders.cmake)

#--------------------------------------------------------

module(LIBRARY)
	dependencies(
		graphics3d	0.*
		window		0.*
	)

	api(opengl)

	sources(modules)
		domain(opengl)

		group(include Headers)
		files(
			mesh.h
			opengl.h
            shader.h
			vertex_layout.h
			uniform_layout.h
		)

		group(src Sources)
		files(
			mesh.cpp
            shader.cpp
			vertex_layout.cpp
			uniform_layout.cpp
		)

if (WIN32)
	files(windows/opengl.cpp)
elseif(UNIX)
	files(linux/opengl.cpp)
else()
	message(FATAL_ERROR "OpenGL integration is not implemented for this platform yet")
endif()

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
        files(code.h)
		shaders_output(embedded.h)
	endsources()
endmodule()

vendor(opengl)

#--------------------------------------------------------
