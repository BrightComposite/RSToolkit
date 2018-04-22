#--------------------------------------------------------
#	asd Scene
#--------------------------------------------------------

project(scene VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(STATIC)
	dependencies(
		graphics3d	0.*
		flow        0.*
		space		0.*
	)

	sources(modules)
		domain(scene)

		group(include Headers)
		files(
			camera.h
			deferred_shading_scene.h
			object.h
			scene.h
		)

		group(src Sources)
		files(
			camera.cpp
			deferred_shading_scene.cpp
			scene.cpp
			)
	endsources()
endmodule()

#--------------------------------------------------------