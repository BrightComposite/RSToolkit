//---------------------------------------------------------------------------

#include <opengl/OpenGL3_3.h>
#include <iostream>

//---------------------------------------------------------------------------

namespace Rapture
{
	namespace OpenGL
	{
		const char * const GLVertexLayout::attributes[] = {
			"position",
			"color",
			"normal",
			"texcoord"
		};

		GLVertexLayout::GLVertexLayout(GLGraphics * graphics, const string & fingerprint) : VertexLayout(fingerprint), graphics(graphics) {}

		void GLVertexLayout::apply()
		{
			//graphics->_vertexAttributes.reserve(elements.size());

			float * pointer = 0;

			for(int i = 0; i < elements.size(); ++i)
			{
				auto units = elements[i]->units;
				glEnableVertexAttribArray(i);
				glVertexAttribPointer(i, units * sizeof(float), GL_FLOAT, GL_FALSE, units, pointer);
				pointer += units;
			}
		}

		void GLVertexLayout::accept(const ShaderCode * code) {}

		void GLGraphics::VertexAttributes::reserve(uint count)
		{
			for(int i = 0; i < _count; ++i)
				glDisableVertexAttribArray(i);

			_count = count;
		}

		GLMeshTrait::GLMeshTrait(GLGraphics * graphics, VertexTopology t) : graphics(graphics)
		{
			switch(t)
			{
				case VertexTopology::Triangles:
					topology = GL_TRIANGLES;
					break;

				case VertexTopology::TriangleStrip:
					topology = GL_TRIANGLE_STRIP;
					break;

				case VertexTopology::Lines:
					topology = GL_LINES;
					break;

				case VertexTopology::LineStrip:
					topology = GL_LINE_STRIP;
					break;
			}
		}

		GLVertexBuffer::GLVertexBuffer(GLGraphics * graphics, VertexLayout * layout, const VertexData & vd) : VertexBuffer(layout, vd)
		{
			if(vd.size % layout->stride != 0)
				throw Exception("Size of vertex buffer doesn't matches its vertex input layout");

			glGenBuffers(1, &handle);
			glBufferData(GL_ARRAY_BUFFER, vd.size, vd.ptr, GL_STATIC_DRAW);
		}

		void GLVertexBuffer::apply() const
		{
			glBindBuffer(GL_ARRAY_BUFFER, handle);
		}

		GLIndexBuffer::GLIndexBuffer(GLGraphics * graphics, const VertexIndices & indices) : IndexBuffer(indices), graphics(graphics)
		{
			glGenBuffers(1, &handle);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), indices.data(), GL_STATIC_DRAW);
		}

		void GLIndexBuffer::apply() const
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
		}

		void GLMesh::draw() const
		{
			graphics->bind(vbuffer);
			glDrawArrays(topology, verticesLocation, vbuffer->verticesCount - verticesLocation);
		}

		void GLIndexedMesh::draw() const
		{
			graphics->bind(vbuffer);
			graphics->bind(ibuffer);
			glDrawElements(topology, ibuffer->size, GL_UNSIGNED_INT, reinterpret_cast<void *>(verticesLocation));
		}

		GLShaderProgram::GLShaderProgram(GLGraphics * graphics, VertexLayout * layout) : graphics(graphics), layout(layout)
		{
			GLuint prog = glCreateProgram();

			if(prog == 0)
				throw Exception("in Shader Operator: Can't create new program!");

			id = prog;
		}

		GLShaderProgram::~GLShaderProgram()
		{
			if(id != 0)
				glDeleteProgram(id);
		}

		void GLShaderProgram::apply() const
		{
			graphics->bind(this);
		}

		GLShader::GLShader(GLShaderProgram * program, const String & path, ShaderCodeState state, GLenum type) : program(program), id(glCreateShader(type))
		{
			if(id == 0)
				throw Exception("in Shader Operator: Can't create new shader!");

			switch(state)
			{
				case ShaderCodeState::Raw:
					compile(path, type);
					break;

				case ShaderCodeState::Compiled:
					//read(path, type);
					break;
			}
		}

		GLShader::GLShader(GLShaderProgram * program, const Handle<ShaderCode> & code, GLenum type) : program(program), id(glCreateShader(type))
		{
			if(id == 0)
				throw Exception("in Shader Operator: Can't create new shader!");

		}

		GLShader::~GLShader()
		{
			if(id != 0)
				glDeleteShader(id);
		}

		void GLShader::compile(const String & filename, GLenum type)
		{
			using namespace boost::filesystem;
			using namespace std;

			path filepath(filename.c_str());

			switch(type)
			{
				case GL_VERTEX_SHADER:
					filepath += ".vs";
					break;

				case GL_FRAGMENT_SHADER:
					filepath += ".fs";
					break;
			}

			if(!exists(filepath))
				throw Exception("File ", filepath.c_str(), " doesn't exist!");

			Handle<ShaderCode> source(static_cast<size_t>(file_size(filepath)));
			ifstream input(filepath.c_str(), ios_base::binary);

			input.read(reinterpret_cast<char *>(source->ptr), source->size);
			input.close();

			if(!initRaw(source, type));
				throw Exception("Can't compile GLSL shader! File: ", filepath.c_str());
		}

		void GLShader::read(const String & filename, GLenum type)
		{
			using namespace boost::filesystem;
			using namespace std;

			path filepath(filename.c_str());

			switch(type)
			{
				case GL_VERTEX_SHADER:
					filepath += ".cvs";
					break;

				case GL_FRAGMENT_SHADER:
					filepath += ".cfs";
					break;
			}

			if(!exists(filepath))
				throw Exception("File ", filepath.c_str(), " doesn't exist!");

			Handle<ShaderCode> source(static_cast<size_t>(file_size(filepath)));
			ifstream input(filepath.c_str(), ios_base::binary);

			input.read(reinterpret_cast<char *>(source->ptr), source->size);
			input.close();

			if(!initCompiled(source, type));
				throw Exception("Can't load compiled GLSL shader! File: ", filepath.c_str());
		}

		bool GLShader::initRaw(const Handle<ShaderCode> & source, GLenum type)
		{
			GLint status;
			GLint size = static_cast<GLint>(source->size);

			glShaderSource(id, 1, (const GLchar **)&source->ptr, &size);
			glCompileShader(id);

			glGetShaderiv(id, GL_COMPILE_STATUS, &status);

			return status == GL_TRUE;
		}

		bool GLShader::initCompiled(const Handle<ShaderCode> & code, GLenum type)
		{
			return false;
		}

		GLVertexShader::GLVertexShader(GLShaderProgram * program, const String & path, ShaderCodeState state) : GLShader(program, path, state, GL_VERTEX_SHADER) {}

		GLVertexShader::GLVertexShader(GLShaderProgram * program, const Handle<ShaderCode> & code) : GLShader(program, code, GL_VERTEX_SHADER) {}

		GLPixelShader::GLPixelShader(GLShaderProgram * program, const String & path, ShaderCodeState state) : GLShader(program, path, state, GL_FRAGMENT_SHADER) {}

		GLPixelShader::GLPixelShader(GLShaderProgram * program, const Handle<ShaderCode> & code) : GLShader(program, code, GL_FRAGMENT_SHADER) {}

		GLImage::GLImage(GLGraphics * graphics, uint width, uint height) : Image(graphics, width, height, ImageFormat::bgra), _ctx(graphics)
		{
			GL11_TEXTURE2D_DESC texDesc;

			ZeroMemory(&texDesc, sizeof(texDesc));

			texDesc.Width = _size.x;
			texDesc.Height = _size.y;
			texDesc.ArraySize = 1;
			texDesc.MipLevels = 1;
			texDesc.BindFlags = GL11_BIND_RENDER_TARGET | GL11_BIND_SHADER_RESOURCE;
			texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = GL11_USAGE_DEFAULT;
			texDesc.CPUAccessFlags = 0;
			texDesc.MiscFlags = 0;

			com_assert(
				graphics->device->CreateTexture2D(&texDesc, nullptr, &_handle)
			);

			com_assert(
				graphics->device->CreateShaderResourceView(_handle, nullptr, &_resource)
			);

			GL11_SAMPLER_DESC sampDesc;
			ZeroMemory(&sampDesc, sizeof(sampDesc));
			sampDesc.Filter = GL11_FILTER_MIN_MAG_MIP_LINEAR;
			sampDesc.AddressU = GL11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressV = GL11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressW = GL11_TEXTURE_ADDRESS_WRAP;
			sampDesc.ComparisonFunc = GL11_COMPARISON_NEVER;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = GL11_FLOAT32_MAX;

			com_assert(
				graphics->device->CreateSamplerState(&sampDesc, &_state)
			);
		}

		GLImage::GLImage(GLGraphics * graphics, const ImageData & data) : Image(graphics, data), _ctx(graphics)
		{
			GL11_TEXTURE2D_DESC texDesc;
			ZeroMemory(&texDesc, sizeof(GL11_TEXTURE2D_DESC));

			texDesc.Width = _size.x;
			texDesc.Height = _size.y;
			texDesc.ArraySize = 1;
			texDesc.MipLevels = 1;
			texDesc.BindFlags = GL11_BIND_SHADER_RESOURCE;
			texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = GL11_USAGE_DEFAULT;
			texDesc.CPUAccessFlags = 0;
			texDesc.MiscFlags = 0;

			switch(_format)
			{
				case ImageFormat::grayscale:
					texDesc.Format = DXGI_FORMAT_R8_UNORM;
					break;

				case ImageFormat::grayscale_alpha:
					texDesc.Format = DXGI_FORMAT_R8G8_UNORM;
					break;

				case ImageFormat::bgra:
					texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					break;

				default:
					texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
					_format = ImageFormat::rgba;
			}

			auto newData = handle<ImageData>();
			data.convert(newData, _format);

			GL11_SUBRESOURCE_DATA texInitData;
			ZeroMemory(&texInitData, sizeof(GL11_SUBRESOURCE_DATA));

			texInitData.pSysMem = newData->ptr;
			texInitData.SysMemPitch = _size.x * preferredBpp(_format);
			texInitData.SysMemSlicePitch = texInitData.SysMemPitch;

			com_assert(
				graphics->device->CreateTexture2D(&texDesc, &texInitData, &_handle)
			);

			com_assert(
				graphics->device->CreateShaderResourceView(_handle, nullptr, &_resource)
			);

			GL11_SAMPLER_DESC sampDesc;
			ZeroMemory(&sampDesc, sizeof(sampDesc));
			sampDesc.Filter = GL11_FILTER_MIN_MAG_MIP_LINEAR;
			sampDesc.AddressU = GL11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressV = GL11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressW = GL11_TEXTURE_ADDRESS_WRAP;
			sampDesc.ComparisonFunc = GL11_COMPARISON_NEVER;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = GL11_FLOAT32_MAX;

			com_assert(
				graphics->device->CreateSamplerState(&sampDesc, &_state)
			);
		}

		void GLImage::apply() const
		{
			_ctx->context->PSSetShaderResources(0, 1, &_resource);
			_ctx->context->PSSetSamplers(0, 1, &_state);
		}

		void GLImage::requestData(ImageData * output) const
		{
			if(output == nullptr)
				throw Exception("Output image buffer should be not-null!");

			ComHandle<IGL11Texture2D> texture;
			GL11_TEXTURE2D_DESC texDesc;

			ComHandle<IGL11Resource> res;
			ComHandle<IGL11Texture2D> ownTexture;

			_handle->GetDesc(&texDesc);

			texDesc.BindFlags = 0;
			texDesc.CPUAccessFlags = GL11_CPU_ACCESS_READ;
			texDesc.Usage = GL11_USAGE_STAGING;

			com_assert(
				_ctx->device->CreateTexture2D(&texDesc, nullptr, &texture)
			);

			_ctx->context->CopyResource(texture, _handle);

			GL11_MAPPED_SUBRESOURCE resource;
			com_assert(
				_ctx->context->Map(texture, 0, GL11_MAP_READ, 0, &resource)
			);

			output->area = _size;
			output->format = ImageFormat::rgba;

			const uint pitch = output->width() * 4;

			output->alloc();

			byte * src = static_cast<byte *>(resource.pData);
			byte * dst = output->ptr;

			for(uint y = 0; y < output->height(); ++y, src += resource.RowPitch, dst += pitch)
				memcpy(dst, src, pitch);

			_ctx->context->Unmap(texture, 0);
		}

		GLUniformAdapter::GLUniformAdapter(GLGraphics * graphics, ShaderType shader, int index, size_t size) : graphics(graphics), index(index)
		{
			GL11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));

			bd.Usage = GL11_USAGE_DEFAULT;
			bd.ByteWidth = static_cast<uint>(size);
			bd.BindFlags = GL11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = 0;

			com_assert(
				graphics->device->CreateBuffer(&bd, NULL, &buffer), "Can't create uniform buffer"
			);

			switch(shader)
			{
				case ShaderType::Vertex:
					graphics->context->VSSetConstantBuffers(index, 1, &buffer);
					break;

				case ShaderType::Pixel:
					graphics->context->PSSetConstantBuffers(index, 1, &buffer);
					break;
			}
		}

		void GLUniformAdapter::update(const void * data)
		{
			graphics->context->UpdateSubresource(buffer, 0, NULL, data, 0, 0);
		}
	}
}

//---------------------------------------------------------------------------
