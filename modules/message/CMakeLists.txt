#--------------------------------------------------------
#	Rapture Message
#--------------------------------------------------------

project(RaptureMessage CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(
	SHARED
	RaptureCore "core"
)

set_module_folder(modules)

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION message
			Channel.h
			Connect.h
			Message.h
			MessageClass.h
			Subject.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION message
			Subject.cpp
		END_SECTION
)

end_module()

#--------------------------------------------------------
