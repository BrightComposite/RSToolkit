#--------------------------------------------------------
#	Rapture Math
#--------------------------------------------------------

project(RaptureMath CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(RaptureCore "core")

	sources(modules)
		domain(math)
		
		group(include Headers)
		files(
			Math.h
			Matrix2x2.h
			Matrix.h
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