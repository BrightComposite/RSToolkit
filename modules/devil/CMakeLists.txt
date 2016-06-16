#--------------------------------------------------------
#	Rapture DevIL adaptation
#--------------------------------------------------------

project(RaptureDevIL CXX)

#--------------------------------------------------------

include(${RAPTURE_ROOT}/tools/project.cmake)

#--------------------------------------------------------

start_module(
	SHARED
	RaptureImageIO "io"
)

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION devil
			DevILImageConverter.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION devil
			DevILImageConverter.cpp
		END_SECTION
)

end_module()

load_third_party(devil)
load_third_party(zlib)

#--------------------------------------------------------