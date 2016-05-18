#--------------------------------------------------------
#	Rapture OpenGL 3.3
#--------------------------------------------------------

project(RaptureOpenGL3_3 CXX)

#--------------------------------------------------------

set(RAPTURE_TOOLS ${RAPTURE_ROOT}/tools)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)
include(${RAPTURE_TOOLS}/shaders.cmake)

#--------------------------------------------------------

start_module()

require_modules(
	RaptureUI "ui"
)

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
	SRC_GROUP include Headers
		START_SECTION graphics
			Provider.h
		END_SECTION

		START_SECTION opengl
			OpenGL3_3.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION opengl
			OpenGL3_3.cpp
		END_SECTION
)

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES} ${SHADERS_INPUT} ${SHADERS_OUTPUT})
load_third_party(opengl3_3)

end_module()

#--------------------------------------------------------
