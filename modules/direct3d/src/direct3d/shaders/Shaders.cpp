//---------------------------------------------------------------------------

#include <direct3d/Direct3D11.h>

#include <direct3d/shaders/2d/rect/vs.inc>
#include <direct3d/shaders/2d/rect/ps.inc>

#include <direct3d/shaders/2d/ellipse/vs.inc>
#include <direct3d/shaders/2d/ellipse/ps.inc>

#include <direct3d/shaders/2d/wired/rect/vs.inc>
#include <direct3d/shaders/2d/wired/rect/ps.inc>

#include <direct3d/shaders/2d/wired/ellipse/vs.inc>
#include <direct3d/shaders/2d/wired/ellipse/ps.inc>

#include <direct3d/shaders/2d/figure/vs.inc>
#include <direct3d/shaders/2d/figure/ps.inc>

#include <direct3d/shaders/2d/image/vs.inc>
#include <direct3d/shaders/2d/image/ps.inc>

#include <direct3d/shaders/2d/text/vs.inc>
#include <direct3d/shaders/2d/text/ps.inc>

#include <direct3d/shaders/3d/color/vs.inc>
#include <direct3d/shaders/3d/color/ps.inc>

#include <direct3d/shaders/3d/multicolor/vs.inc>
#include <direct3d/shaders/3d/multicolor/ps.inc>

#include <direct3d/shaders/3d/texture/vs.inc>
#include <direct3d/shaders/3d/texture/ps.inc>

//---------------------------------------------------------------------------

namespace Rapture
{
	namespace Direct3D
	{
		void D3DGraphics::initShaders()
		{
			auto p2  = getVertexLayout("p2");
			auto p2t = getVertexLayout("p2 t");
			auto p3  = getVertexLayout("p3");
			auto p3t = getVertexLayout("p3 t");
			auto p3c4 = getVertexLayout("p3 c4");

			setShaderProgram<VPShaderProgram>("2d/rect",		  p2,  shader_code_2d_rect_vs,			shader_code_2d_rect_ps);
			setShaderProgram<VPShaderProgram>("2d/ellipse",	      p2t, shader_code_2d_ellipse_vs,		shader_code_2d_ellipse_ps);
			setShaderProgram<VPShaderProgram>("2d/wired/rect",	  p2,  shader_code_2d_wired_rect_vs,	shader_code_2d_wired_rect_ps);
			setShaderProgram<VPShaderProgram>("2d/wired/ellipse", p2t, shader_code_2d_wired_ellipse_vs, shader_code_2d_wired_ellipse_ps);
			setShaderProgram<VPShaderProgram>("2d/figure",		  p2,  shader_code_2d_figure_vs,		shader_code_2d_figure_ps);
			setShaderProgram<VPShaderProgram>("2d/image",		  p2t, shader_code_2d_image_vs,			shader_code_2d_image_ps);
			setShaderProgram<VPShaderProgram>("2d/text",		  p2t, shader_code_2d_text_vs,			shader_code_2d_text_ps);

			setShaderProgram<VPShaderProgram>("3d/color",		  p3,  shader_code_3d_color_vs,			shader_code_3d_color_ps);
			setShaderProgram<VPShaderProgram>("3d/multicolor",	  p3c4, shader_code_3d_multicolor_vs,	shader_code_3d_multicolor_ps);
			setShaderProgram<VPShaderProgram>("3d/texture",		  p3t, shader_code_3d_texture_vs,		shader_code_3d_texture_ps);
		}
	}
}
