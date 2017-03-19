//---------------------------------------------------------------------------

#include <direct3d/shaders/2d/rect/vs.inc>
#include <direct3d/shaders/2d/rect/ps.inc>
#include <direct3d/shaders/2d/ellipse/vs.inc>
#include <direct3d/shaders/2d/ellipse/ps.inc>
#include <direct3d/shaders/2d/figure/vs.inc>
#include <direct3d/shaders/2d/figure/ps.inc>
#include <direct3d/shaders/2d/image/vs.inc>
#include <direct3d/shaders/2d/image/ps.inc>
#include <direct3d/shaders/2d/text/vs.inc>
#include <direct3d/shaders/2d/text/ps.inc>
#include <direct3d/shaders/2d/wired/rect/vs.inc>
#include <direct3d/shaders/2d/wired/rect/ps.inc>
#include <direct3d/shaders/2d/wired/ellipse/vs.inc>
#include <direct3d/shaders/2d/wired/ellipse/ps.inc>
#include <direct3d/shaders/3d/basic/vs.inc>
#include <direct3d/shaders/3d/basic/ps.inc>
#include <direct3d/shaders/3d/color/vs.inc>
#include <direct3d/shaders/3d/color/ps.inc>
#include <direct3d/shaders/3d/multicolor/vs.inc>
#include <direct3d/shaders/3d/multicolor/ps.inc>
#include <direct3d/shaders/3d/texture/vs.inc>
#include <direct3d/shaders/3d/texture/ps.inc>

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
    
	static const RFX_SHADER_CODE_UNIT shader_code_2d_rect[]{{shader_code_2d_rect_vs, RFX_VERTEX_SHADER}, {shader_code_2d_rect_ps, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_2d_ellipse[]{{shader_code_2d_ellipse_vs, RFX_VERTEX_SHADER}, {shader_code_2d_ellipse_ps, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_2d_figure[]{{shader_code_2d_figure_vs, RFX_VERTEX_SHADER}, {shader_code_2d_figure_ps, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_2d_image[]{{shader_code_2d_image_vs, RFX_VERTEX_SHADER}, {shader_code_2d_image_ps, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_2d_text[]{{shader_code_2d_text_vs, RFX_VERTEX_SHADER}, {shader_code_2d_text_ps, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_2d_wired_rect[]{{shader_code_2d_wired_rect_vs, RFX_VERTEX_SHADER}, {shader_code_2d_wired_rect_ps, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_2d_wired_ellipse[]{{shader_code_2d_wired_ellipse_vs, RFX_VERTEX_SHADER}, {shader_code_2d_wired_ellipse_ps, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_3d_basic[]{{shader_code_3d_basic_vs, RFX_VERTEX_SHADER}, {shader_code_3d_basic_ps, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_3d_color[]{{shader_code_3d_color_vs, RFX_VERTEX_SHADER}, {shader_code_3d_color_ps, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_3d_multicolor[]{{shader_code_3d_multicolor_vs, RFX_VERTEX_SHADER}, {shader_code_3d_multicolor_ps, RFX_FRAGMENT_SHADER}};
	static const RFX_SHADER_CODE_UNIT shader_code_3d_texture[]{{shader_code_3d_texture_vs, RFX_VERTEX_SHADER}, {shader_code_3d_texture_ps, RFX_FRAGMENT_SHADER}};

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
