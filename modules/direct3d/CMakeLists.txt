#--------------------------------------------------------
#	Rapture Direct3D11
#--------------------------------------------------------

project(RaptureDirect3D11 CXX)

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

start_sources(modules)
	group(include Headers)
		sources(
			direct3d/
				Direct3D11.h
				..
			graphics/
				Provider.h
		)

	group(src Sources)
		sources(
			direct3d/
				D3DObjects.cpp
				D3DSurfaces.cpp
				Direct3D11.cpp

				shaders/
					Shaders.cpp
		)

	group(resources Resources)
		shaders(HLSL direct3d/shaders
			2d/
				rect/
					vs.fx ps.fx
					..
				ellipse/
					vs.fx ps.fx
					..
				figure/
					vs.fx ps.fx
					..
				image/
					vs.fx ps.fx
					..
				text/
					vs.fx ps.fx
					..
				wired/
					rect/
						vs.fx ps.fx
						..
					ellipse/
						vs.fx ps.fx
						..
			.
			3d/
				basic/
					vs.fx ps.fx
					..
				color/
					vs.fx ps.fx
					..
				multicolor/
					vs.fx ps.fx
					..
				texture/
					vs.fx ps.fx
		)
		
		output(SHADERS_OUTPUT)
	end_shaders()
	
	#group(include Includes)
	#sources(
	#	direct3d/shaders/
	#		${SHADERS_OUTPUT}
	#)
end_sources()

end_module()

load_third_party(direct3d11)

#--------------------------------------------------------