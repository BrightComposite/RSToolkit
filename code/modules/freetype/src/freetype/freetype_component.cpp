//---------------------------------------------------------------------------

#include <freetype/freetype_component.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_TRIGONOMETRY_H

//---------------------------------------------------------------------------

namespace asd
{
    namespace freetype
    {
        static auto & freetype_instance() {
            static struct Instance
            {
                Instance() {
                    FT_Init_FreeType(&ft);
                }
                
                ~Instance() {
                    FT_Done_FreeType(ft);
                }
                
                FT_Library ft;
            } instance;
            
            return instance;
        }
        
        decoder::decoder(gfx::context & ctx) : _ctx(ctx) {}
        
        font & decoder::create(const byte_data & data, const std::string & type, point_unit size) {
            return *_entries.emplace(_entries.end(), data, type, size);
        }
    
        font_entry::font_entry(const byte_data & data, const std::string & type, point_unit size) : _size(size) {
            auto & instance = freetype_instance();
            
            if (FT_New_Memory_Face(instance.ft, data.ptr, static_cast<FT_Long>(data.size), 0, &_face) != 0) {
                throw font_decoding_exception("Can't read font of type \"", type, "\"");
            }
        }
        
        font_entry::font_entry(font_entry && entry) : _face(entry._face), _size(entry._size) {
            entry._face = nullptr;
        }
        
        font_entry::~font_entry() {
            if (_face) {
                FT_Done_Face(_face);
            }
        }
        
        font_entry & font_entry::operator =(font_entry && entry) {
            if (_face) {
                FT_Done_Face(_face);
            }
            
            _face = entry._face;
            _size = entry._size;
            entry._face = nullptr;
            
            return *this;
        }
        
        void font_entry::render(gfx::context & ctx, ulong symbol, const math::int_point & pos) {
            FT_Set_Char_Size(_face, 0, _size.value << 6, 96, 96);
            
            auto glyph_index = FT_Get_Char_Index(_face, symbol);
            
            if (FT_Load_Glyph(_face, glyph_index, FT_LOAD_LINEAR_DESIGN) != 0) {
                throw font_rendering_exception("Can't make symbol: ", symbol);
            }
            
            FT_Glyph glyph;
            
            if (FT_Get_Glyph(_face->glyph, &glyph) != 0) {
                throw font_rendering_exception("Can't make glyph for symbol: ", symbol);
            }
            
            if (FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, 1) != 0) {
                throw font_rendering_exception("Can't render glyph to bitmap!");
            }
            
            auto bit_glyph = reinterpret_cast<FT_BitmapGlyph>(reinterpret_cast<void *>(glyph));
            auto & bmp = bit_glyph->bitmap;
            
            SymbolData data;
            
            data.character = character;
            
            data.area = { bmp.width, bmp.rows };
            
            data.left = bit_glyph->left;
            data.top = _size.value - bit_glyph->top;
            
            data.advance = { glyph->advance.x >> 16, glyph->advance.y >> 16 };
            
            switch (bmp.pixel_mode) {
                case FT_PIXEL_MODE_BGRA:
                    data.format = image_format::bgra;
                    break;
                
                case FT_PIXEL_MODE_GRAY:
                    data.format = image_format::grayscale;
                    break;
                
                case FT_PIXEL_MODE_LCD:
                    data.format = image_format::rgb;
                    break;
                
                default:
                    data.format = image_format::unknown;
            }
        }
    
        math::int_size font_entry::text_size(const text_string & text, const size_constraint & constraint) const {
            return asd::math::int_size(nullptr);
        }
    
        void font_entry::draw(const text_string & text, const text_area & area) const {
        
        }
    }
}

//---------------------------------------------------------------------------
