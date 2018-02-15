//---------------------------------------------------------------------------

#pragma once

#ifndef OPENGL_UNIFORM_LAYOUT_H
#define OPENGL_UNIFORM_LAYOUT_H

//---------------------------------------------------------------------------

#include <container/array_list.h>
#include <container/map.h>
#include <container/data.h>
#include <morph/morph.h>

//---------------------------------------------------------------------------

namespace asd
{
    namespace opengl
    {
        using f32 = float;
        using f32v2 = float2;
        using f32v3 = float3;
        using f32v4 = float4;
        using f32m2 = std::array<float2, 2>;
        using f32m3 = std::array<float3, 3>;
        using f32m4 = std::array<float4, 4>;

        using uniform_type = morph_id_t;

        struct basic_uniform
        {
            morph_origin(basic_uniform)
        };

        template <class T>
        struct generic_uniform : basic_uniform {};

        template <class T>
        uniform_type uniform_id = morph_id<generic_uniform<T>>;

        struct uniform_scheme
        {
            std::string name;
            uniform_type type;
        };

        struct uniform_element
        {
            uniform_element(size_t offset, uniform_type type) : offset(offset), type(type) {}

            int id;
            size_t offset;
            uniform_type type;
        };

        class uniform_layout
        {
        public:
            uniform_layout(const std::string & name) : _name(name) {}
            uniform_layout(const std::string & name, std::initializer_list<uniform_scheme> && list) : _name(name) {
                for (auto & element : list) {
                    add(element);
                }
            }

            api(opengl)
            void add(const uniform_scheme & e);

            api(opengl)
            void add(uniform_scheme && e);

            size_t size() const {
                return _elements.size();
            }

            auto & at(size_t idx) const {
                return _elements.at(idx);
            }

            auto & operator[](size_t idx) const {
                return _elements[idx];
            }

            size_t index(const std::string & name) const {
                return _indices.at(name);
            }

            auto begin() const {
                return _elements.begin();
            }

            auto end() const {
                return _elements.end();
            }

        private:
            std::string _name;
            std::vector<uniform_element> _elements;
            asd::map<std::string, size_t> _indices;
            size_t _current_offset;
        };

        template <class T>
        struct uniform_raw : std::identity<empty> {};

        class uniform_buffer
        {
        public:
            uniform_buffer(const uniform_layout & layout, const data<void> & chunk) : layout(layout), _chunk(chunk) {}

            template <class T, class Raw = template uniform_raw<std::decay_t<T>>::type>
            void set(size_t index, const T & value) {
                auto & element = layout.at(index);
                BOOST_ASSERT_MSG(element.type == uniform_id<Raw>, "The uniform type of the given value must be equal to the uniform type of the element at offset");

                memory<void>::move(_chunk.ptr + element.offset, &Raw(value), sizeof(Raw));
            }

            const uniform_layout & layout;

        private:
            data<void> _chunk;
        };
    }
}

//---------------------------------------------------------------------------
#endif
