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

#include <direct3d/shaders/3d/basic/vs.inc>
#include <direct3d/shaders/3d/basic/ps.inc>

//---------------------------------------------------------------------------

namespace Rapture
{
	namespace Direct3D
	{
		Shaders::Shaders(GraphicContext * ctx) : ctx(ctx)
		{
			auto & p2 = VertexLayout::get("p2");
			auto & p2t = VertexLayout::get("p2 t");
			auto vptype = gettype(VPShaderProgram);

			rectangle		= setCode("2d/rect",		  p2,  vptype, shader_code_2d_rect_vs,			shader_code_2d_rect_ps);
			ellipse			= setCode("2d/ellipse",		  p2t, vptype, shader_code_2d_ellipse_vs,		shader_code_2d_ellipse_ps);
			wired_rectangle = setCode("2d/wired/rect",	  p2,  vptype, shader_code_2d_wired_rect_vs,	shader_code_2d_wired_rect_ps);
			wired_ellipse	= setCode("2d/wired/ellipse", p2t, vptype, shader_code_2d_wired_ellipse_vs, shader_code_2d_wired_ellipse_ps);
			figure			= setCode("2d/figure",		  p2,  vptype, shader_code_2d_figure_vs,		shader_code_2d_figure_ps);
			image			= setCode("2d/image",		  p2t, vptype, shader_code_2d_image_vs,			shader_code_2d_image_ps);
			text			= setCode("2d/text",		  p2t, vptype, shader_code_2d_text_vs,			shader_code_2d_text_ps);

			rectangle->apply();
		}
	}
}
