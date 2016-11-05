#--------------------------------------------------------
#	asd Math
#--------------------------------------------------------

project(asdMath CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(asdCore "core")

	sources(modules)
		domain(math)

		group(include Headers)
		files(
			Box.h
			Frustum.h
			Math.h
			Matrix.h
			Plane.h
			Point.h
			Quaternion.h
			Range.h
			Rect.h
			Transform.h
			Vector.h
		)

		group(src Sources)
		files(Math.cpp)
	endsources()
endmodule()

#--------------------------------------------------------