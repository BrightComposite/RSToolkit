//---------------------------------------------------------------------------

#include <opengl/shaders/2d/color/vs.shader.h>
#include <opengl/shaders/2d/color/fs.shader.h>
#include <opengl/shaders/2d/rect/vs.shader.h>
#include <opengl/shaders/2d/rect/fs.shader.h>
#include <opengl/shaders/2d/ellipse/vs.shader.h>
#include <opengl/shaders/2d/ellipse/fs.shader.h>
#include <opengl/shaders/2d/figure/vs.shader.h>
#include <opengl/shaders/2d/figure/fs.shader.h>
#include <opengl/shaders/2d/image/vs.shader.h>
#include <opengl/shaders/2d/image/fs.shader.h>
#include <opengl/shaders/2d/text/vs.shader.h>
#include <opengl/shaders/2d/text/fs.shader.h>
#include <opengl/shaders/2d/wired/rect/vs.shader.h>
#include <opengl/shaders/2d/wired/rect/fs.shader.h>
#include <opengl/shaders/2d/wired/ellipse/vs.shader.h>
#include <opengl/shaders/2d/wired/ellipse/fs.shader.h>
#include <opengl/shaders/3d/basic/vs.shader.h>
#include <opengl/shaders/3d/basic/fs.shader.h>
#include <opengl/shaders/3d/color/vs.shader.h>
#include <opengl/shaders/3d/color/fs.shader.h>
#include <opengl/shaders/3d/multicolor/vs.shader.h>
#include <opengl/shaders/3d/multicolor/fs.shader.h>
#include <opengl/shaders/3d/texture/vs.shader.h>
#include <opengl/shaders/3d/texture/fs.shader.h>

//---------------------------------------------------------------------------

#include <opengl/shaders/code.h>

//---------------------------------------------------------------------------

namespace asd 
{
    namespace opengl
    {
        namespace shader_code
        {
			static const unit shader_code_2d_color[]{{shader_code_2d_color_vs, GL_VERTEX_SHADER}, {shader_code_2d_color_fs, GL_FRAGMENT_SHADER}};
			static const unit shader_code_2d_rect[]{{shader_code_2d_rect_vs, GL_VERTEX_SHADER}, {shader_code_2d_rect_fs, GL_FRAGMENT_SHADER}};
			static const unit shader_code_2d_ellipse[]{{shader_code_2d_ellipse_vs, GL_VERTEX_SHADER}, {shader_code_2d_ellipse_fs, GL_FRAGMENT_SHADER}};
			static const unit shader_code_2d_figure[]{{shader_code_2d_figure_vs, GL_VERTEX_SHADER}, {shader_code_2d_figure_fs, GL_FRAGMENT_SHADER}};
			static const unit shader_code_2d_image[]{{shader_code_2d_image_vs, GL_VERTEX_SHADER}, {shader_code_2d_image_fs, GL_FRAGMENT_SHADER}};
			static const unit shader_code_2d_text[]{{shader_code_2d_text_vs, GL_VERTEX_SHADER}, {shader_code_2d_text_fs, GL_FRAGMENT_SHADER}};
			static const unit shader_code_2d_wired_rect[]{{shader_code_2d_wired_rect_vs, GL_VERTEX_SHADER}, {shader_code_2d_wired_rect_fs, GL_FRAGMENT_SHADER}};
			static const unit shader_code_2d_wired_ellipse[]{{shader_code_2d_wired_ellipse_vs, GL_VERTEX_SHADER}, {shader_code_2d_wired_ellipse_fs, GL_FRAGMENT_SHADER}};
			static const unit shader_code_3d_basic[]{{shader_code_3d_basic_vs, GL_VERTEX_SHADER}, {shader_code_3d_basic_fs, GL_FRAGMENT_SHADER}};
			static const unit shader_code_3d_color[]{{shader_code_3d_color_vs, GL_VERTEX_SHADER}, {shader_code_3d_color_fs, GL_FRAGMENT_SHADER}};
			static const unit shader_code_3d_multicolor[]{{shader_code_3d_multicolor_vs, GL_VERTEX_SHADER}, {shader_code_3d_multicolor_fs, GL_FRAGMENT_SHADER}};
			static const unit shader_code_3d_texture[]{{shader_code_3d_texture_vs, GL_VERTEX_SHADER}, {shader_code_3d_texture_fs, GL_FRAGMENT_SHADER}};

			static map<std::string, store> shaders {
				{"2d/color", {shader_code_2d_color, shader_code_2d_color_layout}},
				{"2d/rect", {shader_code_2d_rect, shader_code_2d_rect_layout}},
				{"2d/ellipse", {shader_code_2d_ellipse, shader_code_2d_ellipse_layout}},
				{"2d/figure", {shader_code_2d_figure, shader_code_2d_figure_layout}},
				{"2d/image", {shader_code_2d_image, shader_code_2d_image_layout}},
				{"2d/text", {shader_code_2d_text, shader_code_2d_text_layout}},
				{"2d/wired/rect", {shader_code_2d_wired_rect, shader_code_2d_wired_rect_layout}},
				{"2d/wired/ellipse", {shader_code_2d_wired_ellipse, shader_code_2d_wired_ellipse_layout}},
				{"3d/basic", {shader_code_3d_basic, shader_code_3d_basic_layout}},
				{"3d/color", {shader_code_3d_color, shader_code_3d_color_layout}},
				{"3d/multicolor", {shader_code_3d_multicolor, shader_code_3d_multicolor_layout}},
				{"3d/texture", {shader_code_3d_texture, shader_code_3d_texture_layout}}
			};
        }
    }
}

//---------------------------------------------------------------------------
