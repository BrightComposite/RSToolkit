#--------------------------------------------------------
#	Independence
#--------------------------------------------------------

project(Independence CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

start_module()

require_modules(
    RaptureApplication "application"
	RaptureScene       "scene"
	RaptureOpenGL3_3   "opengl"
	RaptureFreeImage   "freeimage"
	RaptureFreeType    "freetype"
)

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
	SRC_GROUP include Headers
		START_SECTION art
			START_SECTION object
				Snake.h
			END_SECTION
		END_SECTION

		START_SECTION utils
			FpsCounter.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION SRC_ROOT
			main.cpp
		END_SECTION
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
load_third_party(vld)

end_module()

#--------------------------------------------------------