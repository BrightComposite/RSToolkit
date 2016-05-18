#--------------------------------------------------------
#	Rapture UI Test
#--------------------------------------------------------

project(RaptureUITest CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

start_module()

require_modules(
    RaptureApplication "application"
	RaptureFreeImage   "freeimage"
	RaptureFreeType    "freetype"
)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    require_module(RaptureDirect3D11 "direct3d")
else()
    require_module(RaptureOpenGL3_3 "opengl")
endif()

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
	SRC_GROUP src Sources
		START_SECTION SRC_ROOT
			ui-test.cpp
		END_SECTION
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
load_third_party(vld)

end_module()

#--------------------------------------------------------
