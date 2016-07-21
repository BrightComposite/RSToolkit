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
			float * pointer = 0;

			for(size_t i = 0; i < elements.size(); ++i)
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
			for(uint i = 0; i < _count; ++i)
				glDisableVertexAttribArray(i);

			_count = count;
		}

		GLVertexBuffer::GLVertexBuffer(GLGraphics * graphics, VertexLayout * layout, const VertexData & vd) : VertexBuffer(layout, vd)
		{
			if(vd.size % layout->stride != 0)
				throw Exception("Size of vertex buffer doesn't matches its vertex input layout");

			glGenBuffers(1, &handle);
			glBindBuffer(GL_ARRAY_BUFFER, handle);
			glBufferData(GL_ARRAY_BUFFER, vd.size, vd.ptr, GL_STATIC_DRAW);
		}

		void GLVertexBuffer::apply() const
		{
			glBindBuffer(GL_ARRAY_BUFFER, handle);
			layout->apply();
		}

		GLIndexBuffer::GLIndexBuffer(GLGraphics * graphics, const VertexIndices & indices) : IndexBuffer(indices), graphics(graphics)
		{
			glGenBuffers(1, &handle);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), indices.data(), GL_STATIC_DRAW);
		}

		void GLIndexBuffer::apply() const
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
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

			glGenVertexArrays(1, &id);
			glBindVertexArray(id);
		}

		GLMesh::GLMesh(GLGraphics * graphics, const Handle<VertexBuffer> & vbuffer, VertexTopology topology, uint verticesLocation) : Mesh(vbuffer, topology, verticesLocation), GLMeshTrait(graphics, topology)
		{
			vbuffer->apply();
		}

		void GLMesh::draw() const
		{
			graphics->bind(this);
			glDrawArrays(topology, verticesLocation, vbuffer->verticesCount - verticesLocation);
		}

		GLIndexedMesh::GLIndexedMesh(GLGraphics * graphics, const Handle<VertexBuffer> & vbuffer, const Handle<IndexBuffer> & ibuffer, VertexTopology topology, uint verticesLocation, uint indicesLocation) : IndexedMesh(vbuffer, ibuffer, topology, verticesLocation, indicesLocation), GLMeshTrait(graphics, topology)
		{
			vbuffer->apply();
			ibuffer->apply();
		}

		void GLIndexedMesh::draw() const
		{
			graphics->bind(this);
			glDrawElements(topology, ibuffer->size, GL_UNSIGNED_SHORT, reinterpret_cast<void *>(indicesLocation));
		}

		GLShaderProgram::GLShaderProgram(GLGraphics * graphics, VertexLayout * layout) : graphics(graphics), layout(layout), id(glCreateProgram())
		{
			if(id == 0)
				throw Exception("Can't create new GLSL program!");

			for(size_t i = 0; i < layout->elements.size(); ++i)
			{
				glBindAttribLocation(id, i, GLVertexLayout::attributes[layout->elements[i]->type]);
			}
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
				throw Exception("Can't create new GLSL shader!");

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

			if(!initRaw(source, type))
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

			if(!initCompiled(source, type))
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

		GLImage::GLImage(GLGraphics * graphics, uint width, uint height) : Image(graphics, width, height, ImageFormat::bgra), _graphics(graphics)
		{
		}

		GLImage::GLImage(GLGraphics * graphics, const ImageData & data) : Image(graphics, data), _graphics(graphics)
		{
			uint w  = umath::round2pow(_size.x);
			uint h = umath::round2pow(_size.y);

			_scale[0] = float(_size.x) / float(w);
			_scale[1] = float(_size.y) / float(h);

			int internalformat;
			int format;

			auto newData = handle<ImageData>();

			switch(_format)
			{
				case ImageFormat::grayscale:
					internalformat = GL_R8;
					format = GL_LUMINANCE;
					break;

				case ImageFormat::grayscale_alpha:
					internalformat = GL_RG8;
					format = GL_LUMINANCE_ALPHA;
					break;

				case ImageFormat::bgra:
					internalformat = GL_RGBA8;
					format = GL_BGRA;
					break;

				default:
					internalformat = GL_RGBA8;
					format = GL_RGBA;
					_format = ImageFormat::rgba;
					data.convert(newData, _format);
			}

			glActiveTexture(GL_TEXTURE0);
			glGenTextures(1, &_id);
			glBindTexture(GL_TEXTURE_2D, _id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			glTexImage2D(GL_TEXTURE_2D, 0, internalformat, w, h, 0, format, GL_UNSIGNED_BYTE, newData->ptr);
		}

		void GLImage::apply() const
		{
			glBindTexture(0, _id);
		}

		void GLImage::requestData(ImageData * output) const
		{
			if(output == nullptr)
				throw Exception("Output image buffer should be not-null!");
		}

		GLUniformAdapter::GLUniformAdapter(GLGraphics * graphics, ShaderType shader, int index, size_t size) : _graphics(graphics), _index(index), _size(size)
		{
			OwnedByteData data(size);

			glGenBuffers(1, &_buffer);
			glBindBufferBase(GL_UNIFORM_BUFFER, _index, _buffer);
			glBufferData(GL_UNIFORM_BUFFER, size, data.ptr, GL_STATIC_DRAW);
		}

		void GLUniformAdapter::update(const void * data)
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, _index, _buffer);
			void * ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
			memcpy(ptr, data, _size);
			glUnmapBuffer(GL_UNIFORM_BUFFER);
		}
	}
}

//---------------------------------------------------------------------------
