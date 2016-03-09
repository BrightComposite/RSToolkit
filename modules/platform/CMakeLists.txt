#--------------------------------------------------------
#	Rapture platform-dependent facilities
#--------------------------------------------------------

project(RapturePlatform CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

require_modules(
	RaptureMath "math"
)

start_module()

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
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

#--------------------------------------------------------

set_project_include_dirs(${PROJECT_NAME}
	${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC RaptureMath)

#--------------------------------------------------------