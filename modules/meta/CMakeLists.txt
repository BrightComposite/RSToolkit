#--------------------------------------------------------
#	Rapture Meta
#--------------------------------------------------------

project(RaptureMeta CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(INLINE)

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION meta
			Adapt.h
			Bitmask.h
			BoolTypes.h
			Flag.h
			Inheritance.h
			Macro.h
			Meta.h
			Preprocessor.h
			Sfinae.h
			Types.h
			UseIf.h
		END_SECTION
)

end_module()

load_third_party(boost COMPONENT preprocessor)

#--------------------------------------------------------
