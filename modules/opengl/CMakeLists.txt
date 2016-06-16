#--------------------------------------------------------
#	Rapture OpenGL 3.3
#--------------------------------------------------------

project(RaptureOpenGL3_3 CXX)

#--------------------------------------------------------

set(RAPTURE_TOOLS ${RAPTURE_ROOT}/tools)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)
include(${RAPTURE_TOOLS}/shaders.cmake)

#--------------------------------------------------------

start_module(
	SHARED
	RaptureUI "ui"
)

set_module_folder(modules)

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION graphics
			Provider.h
		END_SECTION

		START_SECTION opengl
			OpenGL3_3.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION opengl
			OpenGL3_3.cpp
		END_SECTION
)

end_module()

load_third_party(opengl3_3)

#--------------------------------------------------------
