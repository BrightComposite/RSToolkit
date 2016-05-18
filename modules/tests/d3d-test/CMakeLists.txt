#--------------------------------------------------------
#	Rapture Direct3D Test
#--------------------------------------------------------

project(RaptureDirect3DTest CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

start_module()

require_modules(
    RaptureApplication "application"
	RaptureDirect3D11  "direct3d"
	RaptureFreeImage   "freeimage"
	RaptureFreeType    "freetype"
)

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
	SRC_GROUP src Sources
		START_SECTION SRC_ROOT
			d3d-test.cpp
			second.cpp
		END_SECTION
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
load_third_party(vld)

end_module()

#--------------------------------------------------------