#--------------------------------------------------------
#	Rapture Input/Output facilities
#--------------------------------------------------------

project(RaptureIO CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(
	INLINE
	RaptureMeta "meta"
)

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION io
			FileSystem.h
		END_SECTION
)

#--------------------------------------------------------

end_module()

setup_third_party(boost COMPONENT filesystem)
setup_third_party(boost COMPONENT system)

#--------------------------------------------------------