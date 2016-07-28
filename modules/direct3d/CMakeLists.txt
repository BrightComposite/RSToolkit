#--------------------------------------------------------
#	Rapture Direct3D11
#--------------------------------------------------------

project(RaptureDirect3D11 CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)
include(${RAPTURE_TOOLS}/shaders.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(
		RaptureUI "ui"
		)
	
	sources(modules)
		domain(direct3d)
	
		group(include Headers)
		files(
			Direct3D11.h
			
			shaders/
				Shaders.h
			)

		group(src Sources)
		files(
			D3DObjects.cpp
			D3DSurfaces.cpp
			Direct3D11.cpp

			shaders/
				Shaders.cpp
			)

		domain(direct3d/shaders)
		
		group(resources Resources)
		shaders(HLSL
			2d/
				rect/
					vs.fx ps.fx ..
				ellipse/
					vs.fx ps.fx ..
				figure/
					vs.fx ps.fx ..
				image/
					vs.fx ps.fx ..
				text/
					vs.fx ps.fx ..
				wired/
					rect/
						vs.fx ps.fx ..
					ellipse/
						vs.fx ps.fx .
			3d/
				basic/
					vs.fx ps.fx ..
				color/
					vs.fx ps.fx ..
				multicolor/
					vs.fx ps.fx ..
				texture/
					vs.fx ps.fx
			)
			
		group(include Headers)
		domain(direct3d/shaders)
		shaders_output(Shaders.h)
		
		domain(graphics)
		files(Provider.h)
	endsources()
endmodule()

load_third_party(direct3d11)

#--------------------------------------------------------