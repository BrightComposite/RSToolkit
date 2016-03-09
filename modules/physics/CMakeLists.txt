#--------------------------------------------------------
#	Rapture Physics
#--------------------------------------------------------

project(RapturePhysics CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

require_module(
	RaptureScene "scene"
)

start_module()

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

#--------------------------------------------------------

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS})

#--------------------------------------------------------

set_project_include_dirs(${PROJECT_NAME}
	${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC RaptureScene)

#--------------------------------------------------------

target_load_third_party(${PROJECT_NAME} bullet)

#--------------------------------------------------------