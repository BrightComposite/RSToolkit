#--------------------------------------------------------
#	Rapture Application
#--------------------------------------------------------

project(RaptureApplication CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(
	SHARED
	RaptureCore "core"
	RapturePlatform "platform"
)

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION application
			Application.h
			Starter.h
			ThreadLoop.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION application
			Application.cpp
			ThreadLoop.cpp
		END_SECTION
)

end_module()

#--------------------------------------------------------