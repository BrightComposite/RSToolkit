#--------------------------------------------------------
#	asd Scene
#--------------------------------------------------------

project(scene VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(
		ui		0.*
		space	0.*
	)

	sources(modules)
		domain(scene)

		group(include Headers)
		files(
			Camera.h
			DeferredShadingScene.h
			Scene.h
		)

		group(src Sources)
		files(
			Camera.cpp
			DeferredShadingScene.cpp
			Scene.cpp
			)
	endsources()
endmodule()

#--------------------------------------------------------