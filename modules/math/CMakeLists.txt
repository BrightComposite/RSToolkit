#--------------------------------------------------------
#	Rapture Math
#--------------------------------------------------------

project(RaptureMath CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

require_modules(
    RaptureCore "core"
)

start_module()

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
	SRC_GROUP include Headers
		START_SECTION math
			Math.h
			Matrix2x2.h
			Matrix.h
			Point.h
			Quaternion.h
			Range.h
			Rect.h
			Transform.h
			Vector.h
		END_SECTION
			
	SRC_GROUP src Sources
		START_SECTION math
			Math.cpp
		END_SECTION
)

#--------------------------------------------------------
			
add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS})

#--------------------------------------------------------

set_project_include_dirs(
	${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC RaptureCore)

#--------------------------------------------------------