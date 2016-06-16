#--------------------------------------------------------
#	Rapture Scene
#--------------------------------------------------------

project(RaptureScene CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(
	SHARED
	RaptureUI "ui"
)

set_module_folder(modules)

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION scene
			Camera.h
			Scene.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION scene
			Scene.cpp
		END_SECTION
)

end_module()

#--------------------------------------------------------