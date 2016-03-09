#--------------------------------------------------------
#	Independence
#--------------------------------------------------------

project(Independence CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

require_modules(
    RaptureApplication "application"
	RaptureScene "scene"
	RaptureOpenGL3_3 "opengl"
	RaptureFreeImage "freeimage"
	RaptureFreeType "freetype"
)

start_module()

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

#--------------------------------------------------------

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS})

#--------------------------------------------------------

set_project_include_dirs(
	${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		RaptureApplication RaptureScene
		RaptureOpenGL3_3 RaptureFreeImage RaptureFreeType
)

#--------------------------------------------------------

target_load_third_party(${PROJECT_NAME} vld)

#--------------------------------------------------------