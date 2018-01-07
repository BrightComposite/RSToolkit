#--------------------------------------------------------
#	asd graphics
#--------------------------------------------------------

project (graphics VERSION 0.1)

#--------------------------------------------------------

include (${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module (INLINE)
	dependencies (
		math		0.*
		platform	0.*
	)

	sources (modules)
		domain (graphics)

		group (include Headers)
		files (
			color.h
			graphics.h
		)

		# group (src Sources)
		# files (
		# 	graphics.cpp
		# )
	endsources ()
endmodule ()

#--------------------------------------------------------