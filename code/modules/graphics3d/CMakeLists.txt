#--------------------------------------------------------
#	module graphics3d
#--------------------------------------------------------

project(graphics3d VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(STATIC)
	dependencies(
		graphics	0.*
	)

	sources(modules)
		domain(graphics3d)
		
		group(include Headers)
		files(
			vertex_data.h
			vertex_layout.h
		)

		group(src Sources)
		files(
			vertex_data.cpp
		)
	endsources()
endmodule()

#--------------------------------------------------------
