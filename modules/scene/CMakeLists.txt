#--------------------------------------------------------
#	Rapture Scene
#--------------------------------------------------------

project(RaptureScene CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(RaptureUI "ui")

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