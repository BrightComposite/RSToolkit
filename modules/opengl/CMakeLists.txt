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

require_modules(
	RaptureUI "ui"
)

#--------------------------------------------------------

start_module()

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
	SRC_GROUP include Headers
		START_SECTION opengl
			OpenGL3_3.h
		END_SECTION
			
	SRC_GROUP src Sources
		START_SECTION opengl
			OpenGL3_3.cpp
		END_SECTION
)

#--------------------------------------------------------

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES} ${SHADERS_INPUT} ${SHADERS_OUTPUT})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS})

#--------------------------------------------------------

set_project_include_dirs(
	${PROJECT_SOURCE_DIR}/include
)
	
target_link_libraries(${PROJECT_NAME} PUBLIC RaptureUI)

#--------------------------------------------------------

target_load_third_party(${PROJECT_NAME} opengl3_3)

#--------------------------------------------------------