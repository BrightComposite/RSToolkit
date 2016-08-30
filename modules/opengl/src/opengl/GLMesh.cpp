//---------------------------------------------------------------------------

#include <opengl/GLObjects.h>

#include <iostream>

//---------------------------------------------------------------------------

namespace Rapture
{
	namespace OpenGL
	{
		using std::cout;
		using std::endl;
		
		template<>
		class GLMesh<GLMeshType::Plain> : public Mesh
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
			ArrayList<VertexBuffer> buffers;
			uint offset;
			uint verticesCount;
		};

		using GLPlainMesh = GLMesh<GLMeshType::Plain>;

		template<>
		class GLMesh<GLMeshType::Indexed> : public virtual GLPlainMesh
		{
			deny_copy(GLMesh);

		public:
			GLMesh(GLMeshBuilder * builder) : GLPlainMesh(builder), ibuffer(move(builder->_ibuffer)), indicesCount(builder->_indicesCount - builder->_offset)
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

			uint indicesCount = 0;
			Handle<GLIndexBuffer> ibuffer;
		};

		using GLIndexedMesh = GLMesh<GLMeshType::Indexed>;

		template<>
		class GLMesh<GLMeshType::Instanced> : public virtual GLPlainMesh
		{
			deny_copy(GLMesh);

		public:
			GLMesh(GLMeshBuilder * builder) : GLPlainMesh(builder), data(move(builder->_instancedData))
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

			virtual InstancedMeshDataChunk * instance() override
			{
				return data->makeInstance();
			}

			virtual void remove(InstancedMeshDataChunk * chunk) override
			{
				data->releaseInstance(chunk);
			}

			virtual void removeInstances() override
			{
				data->clear();
			}

			Unique<InstancedMeshData> data = nullptr;
			uint buffer;
		};

		using GLInstancedMesh = GLMesh<GLMeshType::Instanced>;

		template<>
		class GLMesh<GLMeshType::InstancedIndexed> : public GLIndexedMesh, public GLInstancedMesh
		{
			deny_copy(GLMesh);

		public:
			GLMesh(GLMeshBuilder * builder) : GLPlainMesh(builder), GLInstancedMesh(builder), GLIndexedMesh(builder) {}
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

			virtual InstancedMeshDataChunk * instance() override final
			{
				return data->makeInstance();
			}

			virtual void remove(InstancedMeshDataChunk * chunk) override final
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

		GLVertexBuffer::GLVertexBuffer(GLGraphics * graphics, VertexLayout * layout, const VertexData & vd) : VertexBuffer(layout, vd)
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

		GLIndexBuffer::GLIndexBuffer(GLGraphics * graphics, const VertexIndices & indices)
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

		MeshBuilder * GLMeshBuilder::buffer(const Handle<VertexBuffer> & buffer)
		{
			if(_verticesCount == 0)
				_verticesCount = buffer->verticesCount;
			else if(_verticesCount != buffer->verticesCount)
				throw Exception("Buffers sizes don't match!");

			_buffers.push_back(buffer);
			return this;
		}

		MeshBuilder * GLMeshBuilder::indices(const VertexIndices & indices)
		{
			set_flag(MeshState::Indexed, _state);

			_ibuffer.init(_graphics, indices);
			_indicesCount = static_cast<uint>(indices.size());

			return this;
		}

		MeshBuilder * GLMeshBuilder::offset(uint offset)
		{
			_offset = offset;
			return this;
		}

		MeshBuilder * GLMeshBuilder::topology(VertexTopology topology)
		{
			switch(topology)
			{
				case VertexTopology::Triangles:
					_topology = GL_TRIANGLES;
					break;

				case VertexTopology::TriangleStrip:
					_topology = GL_TRIANGLE_STRIP;
					break;

				case VertexTopology::Lines:
					_topology = GL_LINES;
					break;

				case VertexTopology::LineStrip:
					_topology = GL_LINE_STRIP;
					break;

				default:
					_topology = 0;
			}

			return this;
		}

		Handle<Mesh> GLMeshBuilder::ready()
		{
			set_flag(MeshState::Ready, _state);

			switch(_state)
			{
				case 1: return handle<GLPlainMesh>(this);
				case 3: return handle<GLIndexedMesh>(this);
				case 5: return handle<GLInstancedMesh>(this);
				case 7: return handle<GLInstancedIndexedMesh>(this);

				default: return nullptr;
			}
		}

		MeshBuilder * GLMeshBuilder::makeInstanced(VertexLayout * layout)
		{
			set_flag(MeshState::Instanced, _state);

			_instancedLayout = layout;
			_instancedData.init(layout->stride);

			return this;
		}
	}
}

//---------------------------------------------------------------------------
