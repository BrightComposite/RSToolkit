#--------------------------------------------------------
#	asd Math
#--------------------------------------------------------

project(math VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(STATIC)
	dependencies(core 0.*)
	
	sources(modules)
		domain(math)
		
		group(include Headers)
		files(
			box.h
			frustum.h
			math.h
			matrix.h
			plane.h
			point.h
			quaternion.h
			range.h
			rect.h
			transform.h
			vector.h
		)
		
		group(src Sources)
		files(math.cpp)
	endsources()
endmodule()

#--------------------------------------------------------