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
			Adapt.h
			Bitmask.h
			BoolTypes.h
			Cast.h
			Flag.h
			Inheritance.h
			Macro.h
			Meta.h
			Preprocessor.h
			Sfinae.h
			Types.h
			UseIf.h
		)
	endsources()
endmodule()

vendor(boost HEADERS preprocessor)

#--------------------------------------------------------
