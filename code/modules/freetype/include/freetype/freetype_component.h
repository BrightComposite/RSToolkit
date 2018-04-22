//---------------------------------------------------------------------------

#pragma once

#ifndef FREETYPE_COMPONENT_H
#define FREETYPE_COMPONENT_H

//---------------------------------------------------------------------------

#include <text/font.h>
#include <freetype/freetype.h>

//---------------------------------------------------------------------------

namespace asd
{
    namespace freetype
    {
        class font_entry: public font
        {
        public:
            api(freetype)
            font_entry(const byte_data & data, const std::string & type, point_unit size);
    
            api(freetype)
            font_entry(font_entry && entry);
    
            api(freetype)
            ~font_entry();
    
            api(freetype)
            font_entry & operator = (font_entry && entry);
    
            api(freetype)
            virtual math::int_size text_size(const text_string & text, const size_constraint & constraint) const override;
            api(freetype)
            virtual void draw(const text_string & text, const text_area & area) const override;
    
        private:
            api(freetype)
            void render(gfx::context & ctx, ulong symbol, const math::int_point & pos);
    
            FT_Face _face;
            point_unit _size;
        };
    
        class decoder : font_decoder
        {
        public:
            api(freetype)
            decoder(gfx::context & ctx);
        
            api(freetype)
            virtual font & create(const byte_data & data, const std::string & type, point_unit size) override;
            
        private:
            gfx::context & _ctx;
            asd::array_list<font_entry> _entries;
        };
    }
}

//---------------------------------------------------------------------------
#endif
