//---------------------------------------------------------------------------

#pragma once

#ifndef GFX3D_UNIFORM_H
#define GFX3D_UNIFORM_H

//---------------------------------------------------------------------------

#include <container/array_list.h>
#include <container/map.h>
#include <container/data.h>
#include <meta/class_id.h>
#include <graphics/graphics.h>
#include <graphics/color.h>
#include <math/vector.h>
#include <math/matrix.h>

//---------------------------------------------------------------------------

namespace asd
{
    namespace gfx3d
    {
        namespace uniform
        {
            class object;
            class block;

            using f32 = float;
            using f32v2 = float2;
            using f32v3 = float3;
            using f32v4 = float4;
            using f32m2 = std::array<float2, 2>;
            using f32m3 = std::array<float3, 3>;
            using f32m4 = std::array<float4, 4>;
            using i32 = int;
            using i32v2 = int2;
            using i32v3 = int3;
            using i32v4 = int4;
            using i32m2 = std::array<int2, 2>;
            using i32m3 = std::array<int3, 3>;
            using i32m4 = std::array<int4, 4>;

            using uniform_type = class_id_t;

            struct basic_uniform
            {
                origin_class(basic_uniform)
            };

            template <class T>
            struct generic_uniform : basic_uniform {};

            template <class T>
            uniform_type uniform_id = class_id<generic_uniform<T>>;

            struct scheme
            {
                template <class Raw>
                static scheme create(const std::string & name) {
                    return{ name, uniform_id<Raw>, sizeof(Raw) };
                }

                std::string name;
                uniform_type type;
                size_t size;
            };

            struct element
            {
                element(size_t offset, uniform_type type) : offset(offset), type(type) {}

                size_t offset;
                uniform_type type;
            };

            class component : public gfx::component
            {
            public:
                virtual ~component() {}

                virtual uniform::object & register_uniform(const std::string & uniform_name, std::initializer_list<uniform::scheme> && list = {}) = 0;
                virtual uniform::object & find_uniform(const std::string & uniform_name) = 0;

                inline uniform::block create_block(const std::string & uniform_name);
            };

            template <class T>
            struct uniform_raw : std::identity<empty> {};

            template <class T, template <class> class Model>
            struct uniform_raw<gfx::color<T, Model>> : std::identity<std::array<T, 4>> {};

            template <class T>
            struct uniform_raw<math::vector<T>> : std::identity<std::array<T, 4>> {};

            template <class T>
            struct uniform_raw<math::matrix<T>> : std::identity<std::array<std::array<T, 4>, 4>> {};

            class block
            {
            public:
                block(int id, const uniform::object & uniform, const asd::data<void> & chunk) :
                    _id(id), uniform(uniform), _chunk(chunk) {}

                block(block && b) noexcept : uniform(b.uniform), _id(b._id), _chunk(std::move(b._chunk)) {
                    b._id = 0;
                }

                block & operator = (block && b) noexcept {
                    std::swap(_id, b._id);
                    std::swap(_chunk, b._chunk);

                    return *this;
                }

                int id() const {
                    return _id;
                }

                const asd::data<void> & chunk() const
                {
                    return _chunk;
                }

                template <bool transpose, class T>
                void set(size_t index, const math::matrix<T> & value);

                template <class T, class Raw = typename uniform_raw<std::decay_t<T>>::type>
                void set(size_t index, const T & value);

                inline void bind() const;

                const uniform::object & uniform;

            private:
                int _id;
                asd::data<void> _chunk;
            };

            class object
            {
            public:
                deny_copy(object);

                object(int id, const std::string & name) : _id(id), _name(name) {}
                object(int id, const std::string & name, std::initializer_list<scheme> && list) : object(id, name) {
                    for (auto & entry : list) {
                        add(entry);
                    }
                }

                object(object && l) : _id(l._id), _name(std::move(l._name)), _indices(std::move(l._indices)), _elements(std::move(l._elements)), _current_offset(l._current_offset) {
                    l._current_offset = 0;
                }

                virtual ~object() {}

                object & operator = (object && l) {
                    std::swap(_id, l._id);
                    std::swap(_name, l._name);
                    std::swap(_indices, l._indices);
                    std::swap(_elements, l._elements);
                    std::swap(_current_offset, l._current_offset);

                    return *this;
                }

                int id() const {
                    return _id;
                }

                size_t byte_size() const {
                    return _current_offset;
                }

                size_t size() const {
                    return _elements.size();
                }

                const std::string & name() const {
                    return _name;
                }

                const element & at(size_t idx) const {
                    return _elements.at(idx);
                }

                const element & operator[](size_t idx) const {
                    return _elements[idx];
                }

                size_t index(const std::string & name) const {
                    return _indices.at(name);
                }

                const element & forName(const std::string & name) const {
                    return at(_indices.at(name));
                }

                void add(const scheme & e)
                {
                    _elements.emplace_back(_current_offset, e.type);
                    _indices.insert({ e.name, _elements.size() - 1 });
                    _current_offset += e.size;
                }

                void add(scheme && e)
                {
                    _elements.emplace_back(_current_offset, e.type);
                    _indices.insert({ std::move(e.name), _elements.size() - 1 });
                    _current_offset += e.size;
                }

                virtual uniform::block create_block() = 0;
                virtual void bind_block(const uniform::block & block) const = 0;
                virtual void free_block(const uniform::block & block) = 0;

                virtual void update() = 0;

            protected:
                int _id;
                std::string _name;
                asd::map<std::string, size_t> _indices;
                asd::array_list<element> _elements;
                size_t _current_offset = 0;
            };

            template <bool transpose, class T>
            void block::set(size_t index, const math::matrix<T> & value) {
                auto & element = uniform.at(index);
                BOOST_ASSERT_MSG(element.type == uniform_id<f32m4>, "The uniform type of the given value must be equal to the uniform type of the element at offset");
                BOOST_ASSERT_MSG(_chunk.size <= element.offset + sizeof(f32m4), "Value overflows buffer size");

                memcpy(static_cast<byte *>(_chunk.ptr) + element.offset, &static_cast<const f32m4 &>(transpose ? value.transposition() : value), sizeof(f32m4));
            }

            template <class T, class Raw>
            void block::set(size_t index, const T & value) {
                auto & element = uniform.at(index);
                BOOST_ASSERT_MSG(element.type == uniform_id<Raw>, "The uniform type of the given value must be equal to the uniform type of the element at offset");
                BOOST_ASSERT_MSG(_chunk.size <= element.offset + sizeof(Raw), "Value overflows buffer size");

                memcpy(static_cast<byte *>(_chunk.ptr) + element.offset, &static_cast<const Raw &>(value), sizeof(Raw));
            }

            inline void block::bind() const {
                uniform.bind_block(*this);
            }

            inline block component::create_block(const std::string & uniform_name) {
                return find_uniform(uniform_name).create_block();
            }
        }
    }
}

//---------------------------------------------------------------------------
#endif
