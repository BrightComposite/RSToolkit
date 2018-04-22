//---------------------------------------------------------------------------

#pragma once

#ifndef IMAGE_H
#define IMAGE_H

//---------------------------------------------------------------------------

#include <graphics/graphics.h>
#include <graphics/coords.h>

//---------------------------------------------------------------------------

namespace asd
{
    namespace gfx
    {
        class image
        {
        public:
            virtual ~image() {}

            inline gfx::coord width() const;
            inline gfx::coord height() const;
            inline const gfx::size & size() const;

            virtual void draw(const gfx::area & area) const = 0;

            virtual boost::optional<owned_byte_data> get_bytes() const = 0;

        private:
            gfx::size _size;
        };

        class image_provider
        {
        public:
            virtual unique<image> load(const std::string & path) const = 0;
            virtual unique<image> load(const byte_data & data, const std::string & format) const = 0;
        };

        inline gfx::coord image::width() const {
            return _size.x;
        }

        inline gfx::coord image::height() const {
            return _size.y;
        }

        inline const gfx::size & image::size() const {
            return _size;
        }
    }
}

//---------------------------------------------------------------------------
#endif
