//---------------------------------------------------------------------------

#include <opengl/GLObjects.h>

#include <iostream>

//---------------------------------------------------------------------------

namespace asd
{
	namespace OpenGL
	{
		using std::cout;
		using std::endl;
		
		template<>
		class GLMesh<GLMeshType::Plain> : public mesh
		{
			deny_copy(GLMesh);

		public:
			GLMesh(GLMeshBuilder * builder) : topology(builder->_topology), buffers(move(builder->_buffers)), offset(builder->_offset), verticesCount(builder->_verticesCount - builder->_offset)
			{
				glGenVertexArrays(1, &id);
				glBindVertexArray(id);

				for(auto & b : buffers)
				{
					auto * layout = b->layout;
					b->apply();
					float * pointer = 0;

					for(size_t i = 0; i < layout->elements.size(); ++i)
					{
						auto units = layout->elements[i]->units;
						auto count = (units - 1) / 4 + 1;

						for(uint j = 0; j < count; ++j, ++attrCount)
						{
							auto u = (j == count - 1) ? ((units - 1) % 4) + 1 : 4;
							glEnableVertexAttribArray(attrCount);
							glVertexAttribPointer(attrCount, u, GL_FLOAT, GL_FALSE, layout->stride, pointer);
							pointer += u;
						}
					}
				}

				glBindVertexArray(0);
			}

			virtual ~GLMesh()
			{
				glDeleteVertexArrays(1, &id);
			}

			virtual void draw() const override
			{
				glBindVertexArray(id);
				glDrawArrays(topology, offset, verticesCount);
				glBindVertexArray(0);
			}
			
			uint id = 0;
			uint attrCount = 0;
			
			uint topology;
			ArrayList<vertex_buffer> buffers;
			uint offset;
			uint verticesCount;
		};

		using GLPlainMesh = GLMesh<GLMeshType::Plain>;

		template<>
		class GLMesh<GLMeshType::Indexed> : public virtual GLPlainMesh
		{
			deny_copy(GLMesh);

		public:
			GLMesh(GLMeshBuilder * builder) : GLPlainMesh(builder), ibuffer(std::move(builder->_ibuffer)), indicesCount(builder->_indicesCount - builder->_offset)
			{
				glBindVertexArray(id);
				ibuffer->apply();
				glBindVertexArray(0);
			}

			virtual ~GLMesh() {}

			virtual void draw() const override
			{
				glBindVertexArray(id);
				glDrawElements(topology, indicesCount, GL_UNSIGNED_SHORT, reinterpret_cast<void *>(static_cast<size_t>(offset)));
				glBindVertexArray(0);
			}
			
			handle<GLIndexBuffer> ibuffer;
			uint indicesCount = 0;
		};

		using GLIndexedMesh = GLMesh<GLMeshType::Indexed>;

		template<>
		class GLMesh<GLMeshType::Instanced> : public virtual GLPlainMesh
		{
			deny_copy(GLMesh);

		public:
			GLMesh(GLMeshBuilder * builder) : GLPlainMesh(builder), data(std::move(builder->_instancedData))
			{
				glBindVertexArray(id);
				glGenBuffers(1, &buffer);
				glBindBuffer(GL_ARRAY_BUFFER, buffer);
				glBufferData(GL_ARRAY_BUFFER, data->contents.size, data->contents.ptr, GL_STATIC_DRAW);

				auto * layout = builder->_instancedLayout;
				float * pointer = 0;

				for(size_t i = 0; i < layout->elements.size(); ++i)
				{
					auto units = layout->elements[i]->units;
					auto count = (units - 1) / 4 + 1;

					for(uint j = 0; j < count; ++j, ++attrCount)
					{
						auto u = (j == count - 1) ? ((units - 1) % 4) + 1 : 4;
						glEnableVertexAttribArray(attrCount);
						glVertexAttribPointer(attrCount, u, GL_FLOAT, GL_FALSE, layout->stride, pointer);
						glVertexAttribDivisor(attrCount, 1);
						pointer += u;
					}
				}

				glBindVertexArray(0);
			}

			virtual ~GLMesh() {}

			virtual void draw() const override
			{
				glBindVertexArray(id);
				glDrawArraysInstanced(topology, offset, verticesCount, static_cast<GLsizei>(data->instances.count()));
				glBindVertexArray(0);
			}

			virtual void updateInstanceData() const override
			{
				glBindBuffer(GL_ARRAY_BUFFER, buffer);
				glBufferData(GL_ARRAY_BUFFER, data->contents.size, data->contents.ptr, GL_STATIC_DRAW);
			}

			virtual instanced_mesh_data_chunk * instance() override
			{
				return data->makeInstance();
			}

			virtual void remove(instanced_mesh_data_chunk * chunk) override
			{
				data->releaseInstance(chunk);
			}

			virtual void removeInstances() override
			{
				data->clear();
			}

			unique<instanced_mesh_data> data = nullptr;
			uint buffer;
		};

		using GLInstancedMesh = GLMesh<GLMeshType::Instanced>;

		template<>
		class GLMesh<GLMeshType::InstancedIndexed> : public GLIndexedMesh, public GLInstancedMesh
		{
			deny_copy(GLMesh);

		public:
			GLMesh(GLMeshBuilder * builder) : GLPlainMesh(builder), GLIndexedMesh(builder), GLInstancedMesh(builder) {}
			virtual ~GLMesh() {}

			virtual void draw() const override final
			{
				glBindVertexArray(id);
				glDrawElementsInstanced(GLInstancedMesh::topology, indicesCount, GL_UNSIGNED_SHORT, reinterpret_cast<void *>(static_cast<size_t>(GLInstancedMesh::offset)), static_cast<GLsizei>(data->instances.count()));
				glBindVertexArray(0);
			}

			virtual void updateInstanceData() const override final
			{
				glBindBuffer(GL_ARRAY_BUFFER, buffer);
				glBufferData(GL_ARRAY_BUFFER, data->contents.size, data->contents.ptr, GL_STATIC_DRAW);
			}

			virtual instanced_mesh_data_chunk * instance() override final
			{
				return data->makeInstance();
			}

			virtual void remove(instanced_mesh_data_chunk * chunk) override final
			{
				data->releaseInstance(chunk);
			}

			virtual void removeInstances() override final
			{
				data->clear();
			}
		};

		using GLInstancedIndexedMesh = GLMesh<GLMeshType::InstancedIndexed>;

//---------------------------------------------------------------------------

		GLVertexBuffer::GLVertexBuffer(GLGraphics * graphics, vertex_layout * layout, const vertex_data & vd) : vertex_buffer(layout, vd)
		{
			if(vd.size() % layout->units != 0)
				throw Exception("Size of vertex buffer doesn't matches its vertex input layout");

			glGenBuffers(1, &handle);
			glBindBuffer(GL_ARRAY_BUFFER, handle);
			glBufferData(GL_ARRAY_BUFFER, vd.size() * sizeof(float), vd.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		GLVertexBuffer::~GLVertexBuffer()
		{
			glDeleteBuffers(1, &handle);
		}

		void GLVertexBuffer::apply() const
		{
			glBindBuffer(GL_ARRAY_BUFFER, handle);
		}

		GLIndexBuffer::GLIndexBuffer(GLGraphics * graphics, const vertex_indices & indices)
		{
			glGenBuffers(1, &handle);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		GLIndexBuffer::~GLIndexBuffer()
		{
			glDeleteBuffers(1, &handle);
		}

		void GLIndexBuffer::apply() const
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
		}

		GLMeshBuilder::GLMeshBuilder(GLGraphics * graphics) : _graphics(graphics) {}
		GLMeshBuilder::~GLMeshBuilder() {}

		mesh_builder * GLMeshBuilder::buffer(const handle<vertex_buffer> & buffer)
		{
			if(_verticesCount == 0)
				_verticesCount = buffer->verticesCount;
			else if(_verticesCount != buffer->verticesCount)
				throw Exception("Buffers sizes don't match!");

			_buffers.push_back(buffer);
			return this;
		}

		mesh_builder * GLMeshBuilder::indices(const vertex_indices & indices)
		{
			set_flag(mesh_state::indexed, _state);

			_ibuffer.init(_graphics, indices);
			_indicesCount = static_cast<uint>(indices.size());

			return this;
		}

		mesh_builder * GLMeshBuilder::offset(uint offset)
		{
			_offset = offset;
			return this;
		}

		mesh_builder * GLMeshBuilder::topology(vertex_topology topology)
		{
			switch(topology)
			{
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

				default:
					_topology = 0;
			}

			return this;
		}

		handle<mesh> GLMeshBuilder::ready()
		{
			set_flag(mesh_state::ready, _state);

			switch(_state)
			{
				case 1: return handle<GLPlainMesh>(this);
				case 3: return handle<GLIndexedMesh>(this);
				case 5: return handle<GLInstancedMesh>(this);
				case 7: return handle<GLInstancedIndexedMesh>(this);

				default: return nullptr;
			}
		}

		mesh_builder * GLMeshBuilder::makeInstanced(vertex_layout * layout)
		{
			set_flag(mesh_state::instanced, _state);

			_instancedLayout = layout;
			_instancedData.init(layout->stride);

			return this;
		}
	}
}

//---------------------------------------------------------------------------
