#--------------------------------------------------------
#	Rapture Core Test
#--------------------------------------------------------

project(RaptureCoreTest CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(
	APPLICATION
    RaptureApplication "application"
    RaptureMessage "message"
)

set_module_folder(tests)

add_module_sources(
	SRC_GROUP src Sources
		START_SECTION SRC_ROOT
			core-test.cpp
		END_SECTION
)

end_module()

load_third_party(vld)

#--------------------------------------------------------