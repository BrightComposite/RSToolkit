#--------------------------------------------------------
#	Rapture platform-dependent facilities
#--------------------------------------------------------

project(RapturePlatform CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

start_module()

require_modules(
	RaptureMath "math"
)

if(WIN32)
set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
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

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})

end_module()

#--------------------------------------------------------