#--------------------------------------------------------
#	Rapture Physics
#--------------------------------------------------------

project(RapturePhysics CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(
	SHARED
	RaptureScene "scene"
)

set_module_folder(modules)

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION physics
			Physics.h
			PhysicalWorld.h
			PlaneObject.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION physics
			Physics.cpp
		END_SECTION
)

end_module()

load_third_party(bullet)

#--------------------------------------------------------