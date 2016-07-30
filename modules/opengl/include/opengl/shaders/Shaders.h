//---------------------------------------------------------------------------

#include <opengl/shaders/2d/rect/vs.shader.inc>
#include <opengl/shaders/2d/rect/fs.shader.inc>
#include <opengl/shaders/2d/ellipse/vs.shader.inc>
#include <opengl/shaders/2d/ellipse/fs.shader.inc>
#include <opengl/shaders/2d/figure/vs.shader.inc>
#include <opengl/shaders/2d/figure/fs.shader.inc>
#include <opengl/shaders/2d/image/vs.shader.inc>
#include <opengl/shaders/2d/image/fs.shader.inc>
#include <opengl/shaders/2d/text/vs.shader.inc>
#include <opengl/shaders/2d/text/fs.shader.inc>
#include <opengl/shaders/2d/wired/rect/vs.shader.inc>
#include <opengl/shaders/2d/wired/rect/fs.shader.inc>
#include <opengl/shaders/2d/wired/ellipse/vs.shader.inc>
#include <opengl/shaders/2d/wired/ellipse/fs.shader.inc>
#include <opengl/shaders/3d/basic/vs.shader.inc>
#include <opengl/shaders/3d/basic/fs.shader.inc>
#include <opengl/shaders/3d/color/vs.shader.inc>
#include <opengl/shaders/3d/color/fs.shader.inc>
#include <opengl/shaders/3d/multicolor/vs.shader.inc>
#include <opengl/shaders/3d/multicolor/fs.shader.inc>
#include <opengl/shaders/3d/texture/vs.shader.inc>
#include <opengl/shaders/3d/texture/fs.shader.inc>

//---------------------------------------------------------------------------

    #define RFX_VERTEX_SHADER   0
    #define RFX_FRAGMENT_SHADER 1
    #define RFX_GEOMETRY_SHADER 2

    struct RFX_SHADER_CODE_UNIT
    {
        template<size_t N>
        constexpr RFX_SHADER_CODE_UNIT(const char (&data)[N], int type) : data(data), size(N), type(type) {}
        template<size_t N>
        constexpr RFX_SHADER_CODE_UNIT(const unsigned char (&data)[N], int type) : data(data), size(N), type(type) {}

        const void * const data;
        const size_t size;
        const int type;
    };

    struct RFX_SHADER_CODE_SET
    {
        template<size_t N>
        constexpr RFX_SHADER_CODE_SET(const char * id, const char * layout, const RFX_SHADER_CODE_UNIT(&units)[N]) : id(id), layout(layout), units(units), count(N) {}

        const char * id;
        const char * layout;
        const RFX_SHADER_CODE_UNIT * const units;
        const int count;
    };
    
	static const RFX_SHADER_CODE_UNIT shader_code_2d_rect[]{{shader_code_2d_rect_vs, RFX_VERTEX_SHADER}, {shader_code_2d_rect_fs, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_2d_ellipse[]{{shader_code_2d_ellipse_vs, RFX_VERTEX_SHADER}, {shader_code_2d_ellipse_fs, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_2d_figure[]{{shader_code_2d_figure_vs, RFX_VERTEX_SHADER}, {shader_code_2d_figure_fs, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_2d_image[]{{shader_code_2d_image_vs, RFX_VERTEX_SHADER}, {shader_code_2d_image_fs, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_2d_text[]{{shader_code_2d_text_vs, RFX_VERTEX_SHADER}, {shader_code_2d_text_fs, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_2d_wired_rect[]{{shader_code_2d_wired_rect_vs, RFX_VERTEX_SHADER}, {shader_code_2d_wired_rect_fs, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_2d_wired_ellipse[]{{shader_code_2d_wired_ellipse_vs, RFX_VERTEX_SHADER}, {shader_code_2d_wired_ellipse_fs, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_3d_basic[]{{shader_code_3d_basic_vs, RFX_VERTEX_SHADER}, {shader_code_3d_basic_fs, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_3d_color[]{{shader_code_3d_color_vs, RFX_VERTEX_SHADER}, {shader_code_3d_color_fs, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_3d_multicolor[]{{shader_code_3d_multicolor_vs, RFX_VERTEX_SHADER}, {shader_code_3d_multicolor_fs, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_3d_texture[]{{shader_code_3d_texture_vs, RFX_VERTEX_SHADER}, {shader_code_3d_texture_fs, RFX_FRAGMENT_SHADER}};

	static const RFX_SHADER_CODE_SET shaders[] {
		{"2d/rect", shader_code_2d_rect_layout, shader_code_2d_rect},
		{"2d/ellipse", shader_code_2d_ellipse_layout, shader_code_2d_ellipse},
		{"2d/figure", shader_code_2d_figure_layout, shader_code_2d_figure},
		{"2d/image", shader_code_2d_image_layout, shader_code_2d_image},
		{"2d/text", shader_code_2d_text_layout, shader_code_2d_text},
		{"2d/wired/rect", shader_code_2d_wired_rect_layout, shader_code_2d_wired_rect},
		{"2d/wired/ellipse", shader_code_2d_wired_ellipse_layout, shader_code_2d_wired_ellipse},
		{"3d/basic", shader_code_3d_basic_layout, shader_code_3d_basic},
		{"3d/color", shader_code_3d_color_layout, shader_code_3d_color},
		{"3d/multicolor", shader_code_3d_multicolor_layout, shader_code_3d_multicolor},
		{"3d/texture", shader_code_3d_texture_layout, shader_code_3d_texture}
	};

//---------------------------------------------------------------------------
