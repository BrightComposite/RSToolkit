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

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION physics
			Physics.h
			PhysicalWorld.h
			PhysicalObject.h
			PlaneObject.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION physics
			PhysicalWorld.cpp
			PhysicalObject.cpp
		END_SECTION
)

end_module()

load_third_party(bullet)

#--------------------------------------------------------