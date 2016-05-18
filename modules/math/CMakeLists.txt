#--------------------------------------------------------
#	Rapture Math
#--------------------------------------------------------

project(RaptureMath CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

start_module()

require_modules(
    RaptureCore "core"
)

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

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})

end_module()

#--------------------------------------------------------