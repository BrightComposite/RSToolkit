#--------------------------------------------------------
#	asd Graphics
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
			Mesh.h
			ScreenCoord.h
			ShaderCode.h
			Shaders.h
			Surface.h
			Uniform.h
			VertexLayout.h

			image/
				Image.h
				ImageData.h

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
				Image.cpp
				ImageData.cpp

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