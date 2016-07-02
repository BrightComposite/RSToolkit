#--------------------------------------------------------
#	Rapture Graphics
#--------------------------------------------------------

project(RaptureGraphics CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

start_module(
	SHARED
	RaptureMath "math"
	RaptureIO "io"
)

set_module_folder(modules)

add_module_sources(
	SRC_GROUP include Headers
		START_SECTION graphics
			Color.h
			GraphicModel.h
			Graphics.h
			Graphics3D.h
			Mesh.h
			ScreenCoord.h
			Shaders.h
			Surface.h
			Uniform.h
			VertexLayout.h

			START_SECTION image
				Image.h
				ImageData.h

				START_SECTION io
					ImageIO.h
				END_SECTION
			END_SECTION

			START_SECTION font
				Font.h

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

end_module()

#--------------------------------------------------------