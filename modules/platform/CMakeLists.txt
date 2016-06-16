#--------------------------------------------------------
#	Rapture platform-dependent facilities
#--------------------------------------------------------

project(RapturePlatform CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(
	SHARED
	RaptureMath "math"
)

if(WIN32)
add_module_sources(
	SRC_GROUP include Headers
		START_SECTION windows
			ComException.h
			ComHandle.h
			RectAdapter.h
			Message.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION windows
			Message.cpp
		END_SECTION
)
elseif(UNIX)

endif()

#--------------------------------------------------------

end_module()

#--------------------------------------------------------