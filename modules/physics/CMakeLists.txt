#--------------------------------------------------------
#	Rapture Physics
#--------------------------------------------------------

project(RapturePhysics CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

start_module()

require_module(
	RaptureScene "scene"
)

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
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

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
load_third_party(bullet)

end_module()

#--------------------------------------------------------