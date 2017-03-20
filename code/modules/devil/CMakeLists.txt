#--------------------------------------------------------
#	asd DevIL adaptation
#--------------------------------------------------------

project(asd-DevIL CXX)

#--------------------------------------------------------

include(${ASD_ROOT}/tools/project.cmake)

#--------------------------------------------------------

start_module(
	SHARED
	asd-ImageIO "io"
)

set_module_folder(modules)

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

vendor(devil)
vendor(zlib)

#--------------------------------------------------------