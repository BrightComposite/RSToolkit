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
		implement(SimpleTechnique::rectangle);
		implement(SimpleTechnique::ellipse);
		implement(SimpleTechnique::wired_rectangle);
		implement(SimpleTechnique::wired_ellipse);
		implement(SimpleTechnique::figure);
		implement(SimpleTechnique::image);
		implement(SimpleTechnique::text);

		ShaderMap Shaders::shaders;

		template<class ShaderType>
		static Handle<SimpleTechnique> setShadersCode(const string & id, const Handle<Vil> & vil, ShaderType type, const initializer_list<ShaderCode> & set)
		{
			return handle<SimpleTechnique>(vil, type, Shaders::setCode(id, set));
		}

		void Shaders::initialize()
		{
			auto & p2 = Vil::get("p2");
			auto & p2t = Vil::get("p2 t");
			auto vptype = gettype(VPShaderProgram);

			SimpleTechnique::rectangle = setShadersCode("2d/rect", p2, vptype, {shader_code_2d_rect_vs,	shader_code_2d_rect_ps});
			SimpleTechnique::ellipse = setShadersCode("2d/ellipse", p2t, vptype, {shader_code_2d_ellipse_vs, shader_code_2d_ellipse_ps});
			SimpleTechnique::wired_rectangle = setShadersCode("2d/wired/rect", p2, vptype, {shader_code_2d_wired_rect_vs, shader_code_2d_wired_rect_ps});
			SimpleTechnique::wired_ellipse = setShadersCode("2d/wired/ellipse", p2t, vptype, {shader_code_2d_wired_ellipse_vs, shader_code_2d_wired_ellipse_ps});
			SimpleTechnique::figure = setShadersCode("2d/figure", p2, vptype, {shader_code_2d_figure_vs, shader_code_2d_figure_ps});
			SimpleTechnique::image = setShadersCode("2d/image", p2t, vptype, {shader_code_2d_image_vs, shader_code_2d_image_ps});
			SimpleTechnique::text = setShadersCode("2d/text", p2t, vptype, {shader_code_2d_text_vs, shader_code_2d_text_ps});

			SimpleTechnique::rectangle->apply();
		}

		void Shaders::free()
		{
			SimpleTechnique::rectangle = nullptr;
			SimpleTechnique::ellipse = nullptr;
			SimpleTechnique::wired_rectangle = nullptr;
			SimpleTechnique::wired_ellipse = nullptr;
			SimpleTechnique::figure = nullptr;
			SimpleTechnique::image = nullptr;
			SimpleTechnique::text = nullptr;

			shaders.clear();
		}
	}
}