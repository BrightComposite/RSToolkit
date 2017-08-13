#--------------------------------------------------------
#	asd graphics
#--------------------------------------------------------

project(graphics VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(STATIC)
	dependencies(
		math		0.*
		io			0.*
		platform	0.*
	)

	sources(modules)
		domain(graphics)

		group(include Headers)
		files(
			Color.h
			GraphicModel.h
			Graphics.h
			Graphics3D.h
			mesh.h
			ScreenCoord.h
			ShaderCode.h
			Shaders.h
			surface.h
			Uniform.h
			vertex_layout.h

			image/
				image.h
				image_data.h

				io/
					ImageIO.h
			.
			font/
				Font.h

				io/
					FontIO.h
			)

		group(src Sources)
		files(
			Graphics.cpp
			Graphics3D.cpp

			image/
				image.cpp
				image_data.cpp

				io/
					ImageIO.cpp
			.
			font/
				Font.cpp

				io/
					FontIO.cpp
			)
	endsources()
endmodule()

#--------------------------------------------------------