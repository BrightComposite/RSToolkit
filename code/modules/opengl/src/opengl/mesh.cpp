//---------------------------------------------------------------------------

#include <opengl/mesh.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace opengl
	{
		template <>
		class generic_mesh<mesh_type::plain> : public opengl::mesh
		{
			deny_copy(generic_mesh);
		
		public:
			generic_mesh(mesh_builder & builder) : mesh(builder._layout), topology(builder._topology), offset(builder._offset), verticesCount(builder._verticesCount - builder._offset) {
				glGenVertexArrays(1, &id);
				glBindVertexArray(id);
				
				for(auto & b : builder._buffers) {
					glBindBuffer(GL_ARRAY_BUFFER, b.handle);
					float * pointer = 0;
					
					for(size_t i = 0; i < builder._layout.elements.size(); ++i) {
						auto units = builder._layout.elements[i].units;
						auto count = (units - 1) / 4 + 1;
						
						for(uint j = 0; j < count; ++j, ++attrCount) {
							auto u = (j == count - 1) ? ((units - 1) % 4) + 1 : 4;
							glEnableVertexAttribArray(attrCount);
							glVertexAttribPointer(attrCount, u, GL_FLOAT, GL_FALSE, builder._layout.units * sizeof(float), pointer);
							pointer += u;
						}
					}
				}
				
				glBindVertexArray(0);
			}
			
			virtual ~generic_mesh() {
				glDeleteVertexArrays(1, &id);
			}
			
			virtual void draw(::asd::opengl::context &) const override {
				glBindVertexArray(id);
				glDrawArrays(topology, offset, verticesCount);
				glBindVertexArray(0);
			}
			
			uint id = 0;
			uint attrCount = 0;
			uint topology;
			uint offset;
			uint verticesCount;
		};
		
		using plain_mesh = generic_mesh<mesh_type::plain>;
		
		template <>
		class generic_mesh<mesh_type::indexed> : public opengl::mesh
		{
			deny_copy(generic_mesh);
		
		public:
			generic_mesh(mesh_builder & builder) : mesh(builder._layout), topology(builder._topology), offset(builder._offset), verticesCount(builder._verticesCount - builder._offset), indicesCount(builder._indicesCount - builder._offset) {
				glGenVertexArrays(1, &id);
				glBindVertexArray(id);
				
				for(auto & b : builder._buffers) {
					glBindBuffer(GL_ARRAY_BUFFER, b.handle);
					float * pointer = 0;
					
					for(size_t i = 0; i < builder._layout.elements.size(); ++i) {
						auto units = builder._layout.elements[i].units;
						auto count = (units - 1) / 4 + 1;
						
						for(uint j = 0; j < count; ++j, ++attrCount) {
							auto u = (j == count - 1) ? ((units - 1) % 4) + 1 : 4;
							glEnableVertexAttribArray(attrCount);
							glVertexAttribPointer(attrCount, u, GL_FLOAT, GL_FALSE, builder._layout.units * sizeof(float), pointer);
							pointer += u;
						}
					}
				}
				
				glBindVertexArray(0);
			}
			
			virtual ~generic_mesh() {}
			
			virtual void draw(opengl::context &) const override {
				glBindVertexArray(id);
				glDrawElements(topology, indicesCount, GL_UNSIGNED_SHORT, reinterpret_cast<void *>(static_cast<size_t>(offset)));
				glBindVertexArray(0);
			}
			
			uint id = 0;
			uint attrCount = 0;
			uint topology;
			uint offset;
			uint verticesCount;
			uint indicesCount;
		};
		
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
				
				auto * layout = builder->_instancedLayout;
				float * pointer = 0;
				
				for(size_t i = 0; i < layout->elements.size(); ++i) {
					auto units = layout->elements[i]->units;
					auto count = (units - 1) / 4 + 1;
					
					for(uint j = 0; j < count; ++j, ++attrCount) {
						auto u = (j == count - 1) ? ((units - 1) % 4) + 1 : 4;
						glEnableVertexAttribArray(attrCount);
						glVertexAttribPointer(attrCount, u, GL_FLOAT, GL_FALSE, layout->stride, pointer);
						glVertexAttribDivisor(attrCount, 1);
						pointer += u;
					}
				}
				
				glBindVertexArray(0);
			}
			
			virtual ~generic_mesh() {}
			
			virtual void draw(opengl::context &) const override {
				glBindVertexArray(id);
				glDrawArraysInstanced(topology, offset, verticesCount, static_cast<GLsizei>(data->instances.count()));
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
				glDrawElementsInstanced(instanced_mesh::topology, indicesCount, GL_UNSIGNED_SHORT, reinterpret_cast<void *>(static_cast<size_t>(instanced_mesh::offset)), static_cast<GLsizei>(data->instances.count()));
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
			
			verticesCount = static_cast<uint>(vd.size() / layout.units);

			glGenBuffers(1, &handle);
			glBindBuffer(GL_ARRAY_BUFFER, handle);
			glBufferData(GL_ARRAY_BUFFER, vd.size() * sizeof(float), vd.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		
		vertex_buffer::vertex_buffer(vertex_buffer && buffer) : handle(buffer.handle), verticesCount(buffer.verticesCount) {
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
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);
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
		
		mesh_builder::mesh_builder(context & graphics, const vertex_layout & layout, const vertex_data & data, vertex_topology topology) : _context(graphics), _layout(layout) {
			switch(topology) {
				case vertex_topology::triangles:
					_topology = GL_TRIANGLES;
					break;

				case vertex_topology::triangle_strip:
					_topology = GL_TRIANGLE_STRIP;
					break;

				case vertex_topology::lines:
					_topology = GL_LINES;
					break;

				case vertex_topology::line_strip:
					_topology = GL_LINE_STRIP;
					break;
			}

			buffer(data);
		}
		
		mesh_builder::~mesh_builder() {}
		
		mesh_builder & mesh_builder::buffer(const vertex_data & data) {
			vertex_buffer buffer(_layout, data);
			
			if(_verticesCount == 0) {
				_verticesCount = buffer.verticesCount;
			} else if(_verticesCount != buffer.verticesCount) {
				throw std::runtime_error("Buffers sizes don't match!");
			}
			
			_buffers.emplace_back(std::move(buffer));
			return *this;
		}
		
		mesh_builder & mesh_builder::indices(const vertex_indices & indices) {
			_ibuffer.init(indices);
			_indicesCount = static_cast<uint>(indices.size());
			
			return *this;
		}
		
		mesh_builder & mesh_builder::offset(uint offset) {
			_offset = offset;
			return *this;
		}
		/*
		mesh_builder * mesh_builder::make_instanced(const vertex_layout * layout) {
			set_flag(mesh_type::instanced, _state);
			
			_instancedLayout = layout;
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
					throw Exception("Invalid builder state");
			}
		}
	}
}

//---------------------------------------------------------------------------
