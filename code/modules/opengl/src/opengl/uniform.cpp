//---------------------------------------------------------------------------

#include <opengl/uniform.h>

//---------------------------------------------------------------------------

namespace asd
{
    namespace opengl
    {
        uniform_buffer::uniform_buffer(int uniform_idx, size_t max_size) :
            data(max_size)
        {
            glGenBuffers(1, &handle);

            glBindBuffer(GL_UNIFORM_BUFFER, handle);
            glBufferData(GL_UNIFORM_BUFFER, data.size, data.ptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            glBindBufferBase(GL_UNIFORM_BUFFER, uniform_idx, handle);
        }

        uniform_buffer::~uniform_buffer()
        {
            if (handle > 0) {
                glDeleteBuffers(1, &handle);
            }
        }

        ptrdiff_t uniform_buffer::get_offset(const asd::data<void> & chunk) const
        {
            return static_cast<byte *>(chunk.ptr) - static_cast<byte *>(data.ptr);
        }

        uniform::object & uniform_component::register_uniform(const std::string & uniform_name, std::initializer_list<uniform::scheme> && list)
        {
            auto idx = static_cast<int>(_uniforms.size());
            _indices.insert_or_assign(uniform_name, idx);
            return **_uniforms.emplace(_uniforms.end(), make::unique<uniform_object>(idx, uniform_name, std::forward<std::initializer_list<uniform::scheme>>(list)));
        }

        uniform::object & uniform_component::find_uniform(const std::string & uniform_name)
        {
            return *_uniforms.at(_indices.at(uniform_name));
        }

        uniform_object::uniform_object(int idx, const std::string & name, std::initializer_list<uniform::scheme> && list) :
            uniform::object(idx, name, std::forward<std::initializer_list<uniform::scheme>>(list))
        {
            _buffers.emplace_back(idx, static_cast<size_t>(glGetInteger(GL_MAX_UNIFORM_BLOCK_SIZE)));
        }

        uniform::block uniform_object::create_block()
        {
            if (_free_list.size() > 0) {
                auto entry = _free_list.top();
                _free_list.pop();
                return { entry.buffer, *this, entry.data };
            }

            auto max_buffer_size = static_cast<size_t>(glGetInteger(GL_MAX_UNIFORM_BLOCK_SIZE));
            auto buffer_offset_alignment = static_cast<size_t>(glGetInteger(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT));

            auto next_offset = aligned_add(_buffers.back().offset, byte_size(), buffer_offset_alignment);

            if (next_offset > max_buffer_size) {
                _buffers.emplace_back(id(), max_buffer_size);
                next_offset = 0;
            }

            auto & pool = _buffers.back();
            data<void> chunk{ static_cast<byte *>(pool.data.ptr) + pool.offset, byte_size() };
            pool.offset = next_offset;

            return { static_cast<int>(_buffers.size() - 1), *this, chunk };
        }

        void uniform_object::bind_block(const uniform::block & block) const
        {
            const uniform_buffer & pool = _buffers.at(block.id());
            glBindBufferRange(GL_UNIFORM_BUFFER, id(), pool.handle, pool.get_offset(block.chunk()), byte_size());
        }

        void uniform_object::free_block(const uniform::block & block)
        {
            _free_list.push({ block.id(), block.chunk() });
        }

        void uniform_object::update()
        {
            for (auto & buffer : _buffers) {
                glBindBufferBase(GL_UNIFORM_BUFFER, id(), buffer.handle);
                glBufferData(GL_UNIFORM_BUFFER, buffer.data.size, buffer.data.ptr, GL_DYNAMIC_DRAW);
            }
        }

        int uniform_component::buffer_index(const std::string & uniform_name) const
        {
            return _indices.at(uniform_name);
        }
    }
}

//---------------------------------------------------------------------------

