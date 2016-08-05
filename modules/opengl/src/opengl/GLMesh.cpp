//---------------------------------------------------------------------------

#include <opengl/GLObjects.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	namespace OpenGL
	{
		struct GLPlainMeshImpl : GLMeshImpl
		{
			virtual ~GLPlainMeshImpl() {}

			virtual void draw(const GLMesh * mesh) const
			{
				glDrawArrays(mesh->topology(), mesh->offset(), mesh->verticesCount());
			}
		};

		struct GLIndexedMeshImpl : virtual GLPlainMeshImpl
		{
			GLIndexedMeshImpl() {}
			virtual ~GLIndexedMeshImpl() {}

			virtual void draw(const GLMesh * mesh) const
			{
				glDrawElements(mesh->topology(), indicesCount, GL_UNSIGNED_SHORT, indicesOffset);
			}

			void * indicesOffset;
			uint indicesCount;
			Handle<GLIndexBuffer> ibuffer;
		};

		struct GLInstancedMeshImpl : virtual GLPlainMeshImpl
		{
			GLInstancedMeshImpl() {}
			virtual ~GLInstancedMeshImpl() {}

			virtual void draw(const GLMesh * mesh) const
			{
				glDrawArraysInstanced(mesh->topology(), mesh->offset(), mesh->verticesCount(), instances.size());
			}

			VertexLayout * layout = nullptr;
			array_list<MeshInstance> instances;
			uint buffer;
			uint offset;
			owned_data<void> data;
		};

		struct GLInstancedIndexedMeshImpl : GLIndexedMeshImpl, GLInstancedMeshImpl
		{
			GLInstancedIndexedMeshImpl(GLIndexedMeshImpl * impl)
			{
				indicesOffset = impl->indicesOffset;
				indicesCount = impl->indicesCount;
				ibuffer = move(impl->ibuffer);
			}

			GLInstancedIndexedMeshImpl(GLInstancedMeshImpl * impl)
			{
				layout = impl->layout;
				instances = move(impl->instances);
				data = move(impl->data);
			}

			virtual ~GLInstancedIndexedMeshImpl() {}

			virtual void draw(const GLMesh * mesh) const final
			{
				glDrawElementsInstanced(mesh->topology(), indicesCount, GL_UNSIGNED_SHORT, indicesOffset, instances.size());
			}
		};

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

		GLMesh::GLMesh(GLGraphics * graphics) : _graphics(graphics)
		{
			glGenVertexArrays(1, &_id);
		}

		GLMesh::~GLMesh()
		{
			glDeleteVertexArrays(1, &_id);
		}

		Mesh * GLMesh::buffer(const Handle<VertexBuffer> & buffer)
		{
			if(_verticesCount == 0)
				_verticesCount = buffer->verticesCount;
			else if(_verticesCount != buffer->verticesCount)
				throw Exception("Buffers sizes don't match!");

			_buffers.push_back(buffer);
			return this;
		}

		Mesh * GLMesh::indices(const VertexIndices & indices, uint offset)
		{
			if(!check_flag(MeshState::Indexed, _state))
			{
				if(check_flag(MeshState::Instanced, _state))
					_impl = unique_handle<GLInstancedIndexedMeshImpl>(_impl->upcast<GLInstancedMeshImpl>());
				else
					_impl = unique_handle<GLIndexedMeshImpl>();
			}

			set_flag(MeshState::Indexed, _state);

			auto impl = _impl->upcast<GLIndexedMeshImpl>();
			impl->ibuffer.init(_graphics, indices);
			impl->indicesOffset = reinterpret_cast<void *>(offset);
			impl->indicesCount = static_cast<uint>(indices.size() - offset);

			_graphics->checkForErrors();

			return this;
		}

		Mesh * GLMesh::topology(VertexTopology topology)
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

		const Mesh * GLMesh::ready()
		{
			glBindVertexArray(_id);

			float * pointer = 0;
			uint k = 0;

			for(auto & b : _buffers)
			{
				auto * layout = b->layout;
				b->apply();

				for(size_t i = 0; i < layout->elements.size(); ++i)
				{
					auto units = layout->elements[i]->units;
					auto count = (units - 1) / 4 + 1;

					for(uint j = 0; j < count; ++j, ++k)
					{
						auto u = (j == count - 1) ? ((units - 1) % 4) + 1 : 4;
						glEnableVertexAttribArray(k);
						glVertexAttribPointer(k, u, GL_FLOAT, GL_FALSE, layout->stride, pointer);
						pointer += u;
					}
				}
			}

			if(check_flag(MeshState::Instanced, _state))
			{
				auto * impl = _impl->upcast<GLInstancedMeshImpl>();
				auto * layout = impl->layout;
				glBindBuffer(GL_ARRAY_BUFFER, impl->buffer);

				for(size_t i = 0; i < layout->elements.size(); ++i)
				{
					auto units = layout->elements[i]->units;
					auto count = (units - 1) / 4 + 1;

					for(uint j = 0; j < count; ++j, ++k)
					{
						auto u = (j == count - 1) ? units % 4 : 4;
						glEnableVertexAttribArray(k);
						glVertexAttribPointer(k, u, GL_FLOAT, GL_FALSE, layout->stride, pointer);
						glVertexAttribDivisor(k, 1);
						pointer += u;
					}
				}
			}

			if(check_flag(MeshState::Indexed, _state))
				_impl->upcast<GLIndexedMeshImpl>()->ibuffer->apply();

			if(_state == 0)
				_impl = unique_handle<GLPlainMeshImpl>();

			glBindVertexArray(0);

			_graphics->checkForErrors();

			set_flag(MeshState::Ready, _state);
			return this;
		}

		Mesh * GLMesh::instanceLayout(VertexLayout * layout)
		{
			if(!check_flag(MeshState::Instanced, _state))
			{
				if(check_flag(MeshState::Indexed, _state))
					_impl = unique_handle<GLInstancedIndexedMeshImpl>(_impl->upcast<GLIndexedMeshImpl>());
				else
					_impl = unique_handle<GLInstancedMeshImpl>();
			}
			
			set_flag(MeshState::Instanced, _state);
			auto impl = _impl->upcast<GLInstancedMeshImpl>();
			impl->layout = layout;
			impl->data.alloc(layout->stride * 16);

			glGenBuffers(1, &impl->buffer);
			glBindBuffer(GL_ARRAY_BUFFER, impl->buffer);
			glBufferData(GL_ARRAY_BUFFER, impl->data.size, impl->data.ptr, GL_STATIC_DRAW);

			return this;
		}

		MeshInstance * GLMesh::instance() const
		{
			if(!check_flag(MeshState::Instanced, _state))
				throw Exception("Can't create instance of a mesh without an instance data layout!");

			auto impl = _impl->upcast<GLInstancedMeshImpl>();
			auto & b = *impl->instances.emplace(impl->instances.end());

			return &b;
		}

		void GLMesh::draw() const
		{
			glBindVertexArray(_id);
			_impl->draw(this);
			glBindVertexArray(0);
		}
	}
}

//---------------------------------------------------------------------------
