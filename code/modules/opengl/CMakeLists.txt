#--------------------------------------------------------
#	asd OpenGL 3.3
#--------------------------------------------------------

project(opengl3_3 VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)
include(${ASD_TOOLS}/shaders.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(ui	0.*)
	api(opengl3_3)

	sources(modules)
		domain(opengl)

		group(include Headers)
		files(
			GLObjects.h
			OpenGL3_3.h
		)

		group(src Sources)
		files(
			GLMesh.cpp
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
		)

		group(include Headers)
		domain(opengl/shaders)
		shaders_output(Shaders.h)

		domain(graphics)
		files(Provider.h)
	endsources()
endmodule()

if(WIN32)
	vendor(opengl3_3)
else()
	find_package(OpenGL REQUIRED)
	find_package(GLEW REQUIRED)
	
	if(NOT OPENGL_FOUND)
		message(FATAL_ERROR "OPENGL not found!")
	endif()
		
	if (NOT GLEW_FOUND)
		message(FATAL_ERROR "GLEW not found!")
	endif()
	
	include_directories(${OpenGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
	link_directories(${OpenGL_LIBRARY_DIRS} ${GLEW_LIBRARY_DIRS})
	add_definitions(${OpenGL_DEFINITIONS} ${GLEW_DEFINITIONS})
		
	target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
endif()
#--------------------------------------------------------
