#--------------------------------------------------------
#	asd Meta
#--------------------------------------------------------

project(meta VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(INLINE)
	sources(modules)
		domain(meta)

		group(include Headers)
		files(
			adapt.h
			bitmask.h
			bool_types.h
			cast.h
			flag.h
			inheritance.h
			macro.h
			meta.h
			preprocessor.h
			sfinae.h
			types.h
			useif.h
		)
	endsources()
endmodule()

#vendor(boost HEADERS preprocessor)

#--------------------------------------------------------
