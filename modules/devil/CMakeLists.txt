#--------------------------------------------------------
#	Rapture DevIL adaptation
#--------------------------------------------------------

project(RaptureDevIL CXX)

#--------------------------------------------------------

include(${RAPTURE_ROOT}/tools/project.cmake)

#--------------------------------------------------------

start_module()

require_module(
	RaptureImageIO "io"
)

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
	SRC_GROUP include Headers
		START_SECTION devil
			DevILImageConverter.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION devil
			DevILImageConverter.cpp
		END_SECTION
)

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
load_third_party(devil)
load_third_party(zlib)

end_module()

#--------------------------------------------------------