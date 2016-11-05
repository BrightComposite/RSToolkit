#--------------------------------------------------------
#	asd Meta
#--------------------------------------------------------

project(asdMeta CXX)

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

load_third_party(boost COMPONENT preprocessor)

#--------------------------------------------------------
