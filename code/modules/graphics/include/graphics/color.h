//---------------------------------------------------------------------------

#pragma once

#ifndef COLOR_H
#define COLOR_H

//---------------------------------------------------------------------------

#include <meta/types.h>

//---------------------------------------------------------------------------

namespace asd
{
    namespace gfx
    {
        template <class T>
        struct rgba
        {
            union
            {
                struct
                {
                    T r, g, b, a;
                };

                std::array<T, 4> components;
            };
        };
        
        template <class T>
        struct argb
        {
            union
            {
                struct
                {
                    T a, r, g, b;
                };

                std::array<T, 4> components;
            };
        };
        
        template <class T>
        struct hsva
        {
            union
            {
                struct
                {
                    T h, s, v, a;
                };

                std::array<T, 4> components;
            };
        };
        
        template <class T>
        struct hsla
        {
            union
            {
                struct
                {
                    T h, s, l, a;
                };

                std::array<T, 4> components;
            };
        };
        
        template <class T>
        struct color_limits {};
        
        template <>
        struct color_limits<int>
        {
            static const int min() {
                return 0;
            }
            
            static const int max() {
                return 255;
            }
        };
        
        template <>
        struct color_limits<float>
        {
            static const float min() {
                return 0.0f;
            }
            
            static const float max() {
                return 1.0f;
            }
        };
        
        template <class T, template <class> class Model = rgba>
        struct color : Model<T>
        {
            using limits = color_limits<T>;
            using model = Model<T>;
            
            color(T x = limits::min(), T y = limits::min(), T z = limits::min(), T alpha = limits::max()) : model {x, y, z, alpha} {}

            operator std::array<T, 4> & () {
                return components;
            }

            operator const std::array<T, 4> & () const {
                return components;
            }

            T & operator [] (size_t idx) {
                return components[idx];
            }

            const T & operator [] (size_t idx) const {
                return components[idx];
            }
        };
        
        using colorf = color<float>;
    }
}

//---------------------------------------------------------------------------
#endif
