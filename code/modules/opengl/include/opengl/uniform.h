//---------------------------------------------------------------------------

#pragma once

#ifndef OPENGL_UNIFORM_H
#define OPENGL_UNIFORM_H

//---------------------------------------------------------------------------

#include <graphics3d/uniform.h>
#include <opengl/opengl.h>
#include <boost/pool/pool.hpp>

//---------------------------------------------------------------------------

namespace asd
{
    namespace opengl
    {
        namespace uniform = gfx3d::uniform;

        struct uniform_chunk
        {
            int buffer;
            asd::data<void> data;
        };

        struct uniform_buffer
        {
            deny_copy(uniform_buffer);
            default_move(uniform_buffer);

            api(opengl)
            uniform_buffer(int uniform_idx, size_t max_size);

            api(opengl)
            ~uniform_buffer();

            api(opengl)
            ptrdiff_t get_offset(const data<void> & data) const;

            GLuint handle = 0;
            size_t offset = 0;
            owned_data<void> data;
        };

        class uniform_object : public gfx3d::uniform::object
        {
            deny_copy(uniform_object);
            default_move(uniform_object);

        public:
            api(opengl)
            uniform_object(int idx, const std::string & name, std::initializer_list<uniform::scheme> && list);

            api(opengl)
            virtual uniform::block create_block() override;

            api(opengl)
            virtual void bind_block(const uniform::block & b) const override;

            api(opengl)
            virtual void free_block(const uniform::block & b) override;

            api(opengl)
            virtual void update() override;

        private:
            array_list<uniform_buffer> _buffers;
            std::stack<uniform_chunk> _free_list;
        };

        class uniform_component : public gfx3d::uniform::component
        {
        public:
            deny_copy(uniform_component);
            default_move(uniform_component);

            uniform_component(opengl::context & ctx) {}
            virtual ~uniform_component() {}

            api(opengl)
            virtual uniform::object & register_uniform(const std::string & uniform_name, std::initializer_list<uniform::scheme> && list = {}) override;

            api(opengl)
            virtual uniform::object & find_uniform(const std::string & uniform_name) override;

            api(opengl)
            int buffer_index(const std::string & uniform_name) const;

        private:
            asd::map<std::string, int> _indices;
            asd::array_list<unique<uniform_object>> _uniforms;
        };
    }
}

//---------------------------------------------------------------------------
#endif
