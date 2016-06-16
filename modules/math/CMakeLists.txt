#--------------------------------------------------------
#	Rapture Math
#--------------------------------------------------------

project(RaptureMath CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(
	SHARED
    RaptureCore "core"
)

add_module_sources(
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

end_module()

#--------------------------------------------------------