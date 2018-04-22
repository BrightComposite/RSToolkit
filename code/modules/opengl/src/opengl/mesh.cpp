//---------------------------------------------------------------------------

#include <opengl/mesh.h>

//---------------------------------------------------------------------------

namespace asd
{
    namespace opengl
    {
        generic_mesh<mesh_type::plain>::generic_mesh(const mesh_data & data) : mesh(data.layout, data.topology, data.offset, data.vertices_count - data.offset) {
            glBindVertexArray(handle);

            for(auto & b : data.buffers) {
                glBindBuffer(GL_ARRAY_BUFFER, b.handle);
                float * pointer = 0;

                for(size_t i = 0; i < layout.elements.size(); ++i) {
                    auto units = layout.elements[i].units;
                    auto count = (units - 1) / 4 + 1;

                    for(size_t j = 0; j < count; ++j, ++attr_count) {
                        GLint u = (j == count - 1) ? ((units - 1) % 4) + 1 : 4;
                        glEnableVertexAttribArray(attr_count);
                        glVertexAttribPointer(attr_count, u, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(layout.units * sizeof(float)), pointer);
                        pointer += u;
                    }
                }
            }

            glBindVertexArray(0);
        }

        void generic_mesh<mesh_type::plain>::draw(::asd::opengl::context &) const {
            glBindVertexArray(handle);
            glDrawArrays(topology, offset, vertices_count);
            glBindVertexArray(0);
        }

        generic_mesh<mesh_type::indexed>::generic_mesh(const mesh_data & data) : mesh(data.layout, data.topology, data.offset, data.vertices_count - data.offset), indices_count(data.indices_count - data.offset) {
            glBindVertexArray(handle);

            for(auto & b : data.buffers) {
                glBindBuffer(GL_ARRAY_BUFFER, b.handle);
                float * pointer = 0;

                for(size_t i = 0; i < layout.elements.size(); ++i) {
                    auto units = layout.elements[i].units;
                    auto count = (units - 1) / 4 + 1;

                    for(size_t j = 0; j < count; ++j, ++attr_count) {
                        auto u = (j == count - 1) ? ((units - 1) % 4) + 1 : 4;
                        glEnableVertexAttribArray(attr_count);
                        glVertexAttribPointer(attr_count, u, GL_FLOAT, GL_FALSE, layout.units * sizeof(float), pointer);
                        pointer += u;
                    }
                }
            }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.index_buffer->handle);
            glBindVertexArray(0);
        }

        void generic_mesh<mesh_type::indexed>::draw(opengl::context &) const {
            glBindVertexArray(handle);
            glDrawElements(topology, indices_count, sizeof(gfx3d::vertex_index_t) == sizeof(uint16_t) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, reinterpret_cast<void *>(static_cast<intptr_t>(offset)));
            glBindVertexArray(0);
        }

        using plain_mesh = generic_mesh<mesh_type::plain>;
        using indexed_mesh = generic_mesh<mesh_type::indexed>;
        /*
        template <>
        class generic_mesh<mesh_type::instanced> : public virtual plain_mesh
        {
            deny_copy(generic_mesh);
        
        public:
            generic_mesh(mesh_builder * builder) : plain_mesh(builder), data(std::move(builder->_instancedData)) {
                glBindVertexArray(id);
                glGenBuffers(1, &buffer);
                glBindBuffer(GL_ARRAY_BUFFER, buffer);
                glBufferData(GL_ARRAY_BUFFER, data->contents.size, data->contents.ptr, GL_STATIC_DRAW);
                
                auto * layout = builder->instanced_layout;
                float * pointer = 0;
                
                for(size_t i = 0; i < layout->elements.size(); ++i) {
                    auto units = layout->elements[i]->units;
                    auto count = (units - 1) / 4 + 1;
                    
                    for(uint j = 0; j < count; ++j, ++attr_count) {
                        auto u = (j == count - 1) ? ((units - 1) % 4) + 1 : 4;
                        glEnableVertexAttribArray(attr_count);
                        glVertexAttribPointer(attr_count, u, GL_FLOAT, GL_FALSE, layout->stride, pointer);
                        glVertexAttribDivisor(attr_count, 1);
                        pointer += u;
                    }
                }
                
                glBindVertexArray(0);
            }
            
            virtual ~generic_mesh() {}
            
            virtual void draw(opengl::context &) const override {
                glBindVertexArray(id);
                glDrawArraysInstanced(topology, offset, vertices_count, static_cast<GLsizei>(data->instances.count()));
                glBindVertexArray(0);
            }
            
            virtual void updateInstanceData() const override {
                glBindBuffer(GL_ARRAY_BUFFER, buffer);
                glBufferData(GL_ARRAY_BUFFER, data->contents.size, data->contents.ptr, GL_STATIC_DRAW);
            }
            
            virtual instanced_mesh_data_chunk * instance() override {
                return data->makeInstance();
            }
            
            virtual void remove(instanced_mesh_data_chunk * chunk) override {
                data->releaseInstance(chunk);
            }
            
            virtual void removeInstances() override {
                data->clear();
            }
            
            unique<instanced_mesh_data> data = nullptr;
            uint buffer;
        };
        
        using instanced_mesh = generic_mesh<mesh_type::instanced>;
        
        template <>
        class generic_mesh<mesh_type::instanced_indexed> : public indexed_mesh, public instanced_mesh
        {
            deny_copy(generic_mesh);
        
        public:
            generic_mesh(mesh_builder * builder) : plain_mesh(builder), indexed_mesh(builder), instanced_mesh(builder) {}
            
            virtual ~generic_mesh() {}
            
            virtual void draw() const override final {
                glBindVertexArray(id);
                glDrawElementsInstanced(instanced_mesh::topology, indices_count, GL_UNSIGNED_SHORT, reinterpret_cast<void *>(static_cast<size_t>(instanced_mesh::offset)), static_cast<GLsizei>(data->instances.count()));
                glBindVertexArray(0);
            }
            
            virtual void updateInstanceData() const override final {
                glBindBuffer(GL_ARRAY_BUFFER, buffer);
                glBufferData(GL_ARRAY_BUFFER, data->contents.size, data->contents.ptr, GL_STATIC_DRAW);
            }
            
            virtual instanced_mesh_data_chunk * instance() override final {
                return data->makeInstance();
            }
            
            virtual void remove(instanced_mesh_data_chunk * chunk) override final {
                data->releaseInstance(chunk);
            }
            
            virtual void removeInstances() override final {
                data->clear();
            }
        };
        
        using instanced_indexed_mesh = generic_mesh<mesh_type::instanced_indexed>;
*/
//---------------------------------------------------------------------------
        
        vertex_buffer::vertex_buffer(const vertex_layout & layout, const vertex_data & vd) {
            if(vd.size() % layout.units != 0) {
                throw Exception("Size of vertex buffer doesn't matches its vertex input layout");
            }
            
            vertices_count = static_cast<GLsizei>(vd.size() / layout.units);

            glGenBuffers(1, &handle);
            glBindBuffer(GL_ARRAY_BUFFER, handle);
            glBufferData(GL_ARRAY_BUFFER, vd.size() * sizeof(float), vd.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        
        vertex_buffer::vertex_buffer(vertex_buffer && buffer) : handle(buffer.handle), vertices_count(buffer.vertices_count) {
            buffer.handle = 0;
        }
        
        vertex_buffer::~vertex_buffer() {
            if(handle > 0) {
                glDeleteBuffers(1, &handle);
            }
        }
        
        index_buffer::index_buffer(const vertex_indices & indices) {
            glGenBuffers(1, &handle);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(gfx3d::vertex_index_t), indices.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        
        index_buffer::index_buffer(index_buffer && buffer) : handle(buffer.handle) {
            buffer.handle = 0;
        }
        
        index_buffer::~index_buffer() {
            if(handle > 0) {
                glDeleteBuffers(1, &handle);
            }
        }
        
        mesh_builder::mesh_builder(context & graphics, const vertex_layout & layout, const vertex_data & data, vertex_topology topology) : mesh_data(layout), _context(graphics) {
            switch(topology) {
                case vertex_topology::triangles:
                    this->topology = GL_TRIANGLES;
                    break;

                case vertex_topology::triangle_strip:
                    this->topology = GL_TRIANGLE_STRIP;
                    break;

                case vertex_topology::lines:
                    this->topology = GL_LINES;
                    break;

                case vertex_topology::line_strip:
                    this->topology = GL_LINE_STRIP;
                    break;
            }

            buffer(data);
        }
        
        mesh_builder::~mesh_builder() {}
        
        mesh_builder & mesh_builder::buffer(const vertex_data & data) {
            vertex_buffer buffer(layout, data);
            
            if(vertices_count == 0) {
                vertices_count = buffer.vertices_count;
            } else if(vertices_count != buffer.vertices_count) {
                throw std::runtime_error("Buffers sizes don't match!");
            }
            
            buffers.emplace_back(std::move(buffer));
            return *this;
        }
        
        mesh_builder & mesh_builder::indices(const vertex_indices & indices) {
            set_flag(mesh_type::indexed, _type);

            index_buffer.emplace(indices);
            indices_count = static_cast<GLsizei>(indices.size());
            
            return *this;
        }
        
        mesh_builder & mesh_builder::offset(uint offset) {
            mesh_data::offset = offset;
            return *this;
        }
        /*
        mesh_builder * mesh_builder::make_instanced(const vertex_layout * layout) {
            set_flag(mesh_type::instanced, _state);
            
            instanced_layout = layout;
            _instancedData.init(layout->stride);
            
            return this;
        }*/
        
        handle<mesh> mesh_builder::build() {
            switch(_type) {
                case mesh_type::plain:
                    return {make::handle<plain_mesh>(*this)};
                
                case mesh_type::indexed:
                    return {make::handle<indexed_mesh>(*this)};
                
                    /*case 5:
                    return handle<instanced_mesh>(this);
                    case 7:
                    return handle<instanced_indexed_mesh>(this);*/

                default:
                    throw std::runtime_error("Invalid builder state");
            }
        }
    }
}

//---------------------------------------------------------------------------
