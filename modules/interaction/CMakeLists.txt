#--------------------------------------------------------
#	Rapture Interaction
#--------------------------------------------------------

project(RaptureInteraction CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(
	SHARED
	RaptureUI "ui"
)

set_module_folder(modules)

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION interaction
			MouseManipulator.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION interaction
			MouseManipulator.cpp
		END_SECTION
)

end_module()

#--------------------------------------------------------
