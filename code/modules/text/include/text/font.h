//---------------------------------------------------------------------------

#pragma once

#ifndef TEXT_FONT_H
#define TEXT_FONT_H

//---------------------------------------------------------------------------

#include <container/data.h>
#include <math/rect.h>
#include <graphics/graphics.h>
#include <io/filesystem.h>
#include <boost/variant.hpp>

//---------------------------------------------------------------------------

namespace asd
{
    using font_handle = int;
    
    struct pixel_unit { int value; };
    struct point_unit { int value; };
    
    enum class resize_mode
    {
        no_resize,
        stretch,
        fit,
        crop
    };
    
    struct no_constraint {};
    struct width_constraint { int value; };
    struct height_constraint { int value; };
    struct ratio_constraint { float value; };
    
    using text_string = boost::variant<
        const char * const &,
        const std::string &,
        const std::wstring &,
        const std::u16string &,
        const std::u32string &
    >;
    
    using size_constraint = boost::variant<
        const no_constraint &,
        const width_constraint &,
        const height_constraint &,
        const ratio_constraint &
    >;
    
    struct text_area
    {
        math::int_rect area;
        resize_mode mode;
    };
    
    exception_class(font_decoder_not_found_exception, "Font decoder not found");
    
    exception_class(font_decoding_exception, "Font decoding exception");
    
    exception_class(font_rendering_exception, "Font rendering exception");
    
    class font
    {
    public:
        virtual ~font() {}
        
        virtual void draw(const text_string & text, const text_area & area) const = 0;
        
        void draw(const text_string & text, const math::int_rect & area, resize_mode mode = resize_mode::fit) const {
            draw(text, text_area{ area, mode });
        }
        
        void draw(const text_string & text, const math::int_point & pos) const {
            draw(text, text_area{ math::int_rect{ pos, pos }, resize_mode::no_resize });
        }
        
        void draw(const text_string & text, int x, int y) const {
            draw(text, text_area{ math::int_rect{ x, y, x, y }, resize_mode::no_resize });
        }
        
        virtual math::int_size text_size(const text_string & text, const size_constraint & constraint) const = 0;
        
        math::int_size text_size(const text_string & text) const {
            return text_size(text, no_constraint{});
        }
    };
    
    class font_decoder
    {
    public:
        virtual font & create(const byte_data & data, const std::string & type, point_unit size) = 0;
    };
    
    class font_component : public gfx::component
    {
    public:
        virtual point_unit to_points(pixel_unit size) const {
            return { size.value * 72 / 96 };
        }
        
        font & create(const byte_data & data, const std::string & type, point_unit size)
        {
            auto it = _keys.find(type);
            
            if (it == _keys.end()) {
                throw font_decoder_not_found_exception();
            }
            
            return it->second->create(data, type, size);
        }
        
        font & create(const boost::filesystem::path & path, point_unit size) {
            return create(fs::read_contents(path), path.extension().string(), size);
        }
        
        /**
         * @brief
         * 	Obtains font interface from context
         *
         * @param context
         * @return instance of font interface
         *
         * @throws interface_not_found_exception
         */
        static font_component & from(gfx::context & context) {
            return asd::get<font_component>(context);
        }
    
    protected:
        std::map<std::string, unique<font_decoder>> _keys;
    };
}

//---------------------------------------------------------------------------
#endif
