#--------------------------------------------------------
#	Rapture Small Test
#--------------------------------------------------------

project(RaptureSmallTest CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(
	APPLICATION
    RaptureCore "core"
)

add_module_sources(
	SRC_GROUP src Sources
		START_SECTION SRC_ROOT
			small-test.cpp
		END_SECTION
)

end_module()

load_third_party(vld)

#--------------------------------------------------------
