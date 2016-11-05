#--------------------------------------------------------
#	asd Scene
#--------------------------------------------------------

project(asd-Scene CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(asd-UI "ui")
	dependencies(asd-Space "space")

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