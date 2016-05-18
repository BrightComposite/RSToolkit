#--------------------------------------------------------
#	Rapture Graphics
#--------------------------------------------------------

project(RaptureGraphics CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

start_module()

require_modules(
	RaptureMath "math"
	RaptureIO "io"
)

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
	SRC_GROUP include Headers
		START_SECTION graphics
			Color.h
			Graphics.h
			Graphics3D.h
			ScreenCoord.h
			Shaders.h
			Uniforms.h
			VertexLayout.h

			START_SECTION image
				Image.h
				ImageData.h

				START_SECTION io
					ImageIO.h
				END_SECTION
			END_SECTION

			START_SECTION text
				Font.h
				Text.h

				START_SECTION io
					FontIO.h
				END_SECTION
			END_SECTION
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION graphics
			Graphics.cpp
			Graphics3D.cpp

			START_SECTION image
				Image.cpp
				ImageData.cpp

				START_SECTION io
					ImageIO.cpp
				END_SECTION
			END_SECTION

			START_SECTION text
				Font.cpp

				START_SECTION io
					FontIO.cpp
				END_SECTION
			END_SECTION
		END_SECTION
)

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})

end_module()

#--------------------------------------------------------