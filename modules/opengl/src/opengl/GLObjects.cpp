//---------------------------------------------------------------------------

#include <opengl/GLObjects.h>

#include <iostream>

//---------------------------------------------------------------------------

#ifdef GL_DEEP_DEBUG
#undef GLEW_GET_FUN

template<class T>
static T get_gl_function(const char * name, T func)
{
	using namespace std;
	cout << name << endl;
	return func;
}

#define GLEW_GET_FUN(x) get_gl_function(#x, x)
#endif

namespace Rapture
{
	namespace OpenGL
	{
		GLShaderProgram::GLShaderProgram(GLGraphics * graphics, const string & filename, VertexLayout * layout, ShaderCodeState state) : GLShaderProgram(graphics, layout)
		{
			if(id == 0)
				throw Exception("Can't create new GLSL program!");
		}

		GLShaderProgram::GLShaderProgram(GLGraphics * graphics, const string & key, VertexLayout * layout, const ShaderCodeSet & codeSet, ShaderCodeState state) : GLShaderProgram(graphics, layout)
		{
			if(id == 0)
				throw Exception("Can't create new GLSL program (", key, ")");

			using namespace std;

		#ifdef GL_DEBUG
			cout << "Compile shader program \"" << key << "\"" << endl;
		#endif

			for(auto & c : codeSet)
			{
				auto & shader = _shaders[c.first];
				shader.init(this, c.second, state, glShaderType(c.first));
				glAttachShader(id, shader->id);
			}

			uint k = 0;

			for(size_t i = 0; i < layout->elements.size(); ++i)
			{
				auto & e = layout->elements[i];
				glBindAttribLocation(id, k, e->semantic);
				k += (e->units - 1) / 4 + 1;
			}

			glLinkProgram(id);

			int status;
			glGetProgramiv(id, GL_LINK_STATUS, &status);

		#ifdef GL_DEBUG
			int l = 0;
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &l);

			if(l > 1)
			{
				owned_data<char> buffer(l + 1);

				glGetProgramInfoLog(id, l, nullptr, buffer.ptr);
				buffer[l] = '\0';

				cout << buffer.ptr << endl;
			}
		#endif

			if(status == GL_FALSE)
				throw Exception("Can't link shader program!");

			for(auto & b : graphics->_uniformBindings)
			{
				auto block = glGetUniformBlockIndex(id, b.name);

				if(block != GL_INVALID_INDEX)
					glUniformBlockBinding(id, block, b.index);
			}

			graphics->checkForErrors();
		}

		GLShaderProgram::GLShaderProgram(GLGraphics * graphics, VertexLayout * layout) : graphics(graphics), id(glCreateProgram()), layout(layout) {}

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

		GLShader::GLShader(GLShaderProgram * program, const Handle<ShaderCode> & code, ShaderCodeState state, GLenum type) : program(program), id(glCreateShader(type))
		{
			if(id == 0)
				throw Exception("in Shader Operator: Can't create new shader!");

			switch(state)
			{
				case ShaderCodeState::Raw:
					initRaw(code, type);
					break;

				case ShaderCodeState::Compiled:
					//initCompiled(code, type);
					break;
			}
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

			initRaw(source, type);
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

			initCompiled(source, type);
		}

		void GLShader::initRaw(const Handle<ShaderCode> & source, GLenum type)
		{
			using namespace std;

			GLint size = static_cast<GLint>(source->size);

		#ifdef GL_DEBUG
			switch(type)
			{
				case GL_VERTEX_SHADER:
					cout << "Compile vertex shader" << endl;
					break;

				case GL_FRAGMENT_SHADER:
					cout << "Compile fragment shader" << endl;
					break;

				default:
					cout << "Compile shader" << endl;
			}
		#endif

			glShaderSource(id, 1, (const GLchar **)&source->ptr, &size);
			glCompileShader(id);

			GLint status;
			glGetShaderiv(id, GL_COMPILE_STATUS, &status);

			if(status == GL_FALSE)
			{
			#ifdef GL_DEBUG
				using namespace std;

				int l = 0;
				glGetShaderiv(id, GL_INFO_LOG_LENGTH, &l);

				if(l != 0)
				{
					owned_data<char> buffer(l + 1);

					glGetShaderInfoLog(id, l, nullptr, buffer.ptr);
					buffer[l] = '\0';

					cout << buffer.ptr << endl;
				}
			#endif

				throw Exception("Can't compile GLSL shader!");
			}
		}

		void GLShader::initCompiled(const Handle<ShaderCode> & code, GLenum type)
		{
		}

		GLImage::GLImage(GLGraphics * graphics, uint width, uint height) : Image(graphics, width, height, ImageFormat::bgra), _graphics(graphics)
		{
		}

		GLImage::GLImage(GLGraphics * graphics, const ImageData & data) : Image(graphics, data), _graphics(graphics)
		{
			uint w = umath::round2pow(_size.x);
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
					format = GL_RED;
					break;

				case ImageFormat::grayscale_alpha:
					internalformat = GL_RG8;
					format = GL_RG;
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
			graphics->checkForErrors();

			glTexImage2D(GL_TEXTURE_2D, 0, internalformat, w, h, 0, format, GL_UNSIGNED_BYTE, newData->ptr);
			graphics->checkForErrors();
		}

		void GLImage::apply() const
		{
			glBindTexture(GL_TEXTURE_2D, _id);
			_graphics->checkForErrors();
		}

		void GLImage::requestData(ImageData * output) const
		{
			if(output == nullptr)
				throw Exception("Output image buffer should be not-null!");
		}

		GLUniformAdapter::GLUniformAdapter(GLGraphics * graphics, ShaderType shader, int index, size_t size) : UniformAdapter(index, size), _graphics(graphics), _offset(0)
		{
			glGenBuffers(1, &_common);
			glBindBufferBase(GL_UNIFORM_BUFFER, _index, _common);
			glBufferData(GL_UNIFORM_BUFFER, _size, nullptr, GL_DYNAMIC_DRAW);
		}

		GLUniformAdapter::~GLUniformAdapter()
		{
			glDeleteBuffers(1, &_common);

			for(auto & b : _buffers)
				glDeleteBuffers(1, &b.handle);
		}

		void GLUniformAdapter::update(const void * data)
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, _index, _common);
			glBufferData(GL_UNIFORM_BUFFER, _size, data, GL_DYNAMIC_DRAW);
		}

		void GLUniformAdapter::update()
		{
			for(auto & b : _buffers)
			{
				glBindBufferBase(GL_UNIFORM_BUFFER, _index, b.handle);
				glBufferData(GL_UNIFORM_BUFFER, b.data.size, b.data.ptr, GL_DYNAMIC_DRAW);
			}
		}

		void GLUniformAdapter::append(UniformData & data)
		{
			static const auto max_buffer_size = static_cast<uint>(glGetInteger(GL_MAX_UNIFORM_BLOCK_SIZE));
			static const auto buffer_offset_alignment = static_cast<uint>(glGetInteger(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT));

			if(_buffers.size() == 0 || _offset + _size > max_buffer_size)
			{
				_offset = 0;
				_buffers.emplace_back();

				auto & b = _buffers.back();
				b.data.alloc(max_buffer_size);

				glGenBuffers(1, &b.handle);
				glBindBufferBase(GL_UNIFORM_BUFFER, _index, b.handle);
				glBufferData(GL_UNIFORM_BUFFER, b.data.size, b.data.ptr, GL_DYNAMIC_DRAW);
			}

			auto & b = _buffers.back();

			data.buffer  = _buffers.size() - 1;
			data.offset  = _offset;
			data.data    = {b.data.ptr + _offset, _size};
			data.adapter = this;

			_offset += _size + buffer_offset_alignment - 1;
			_offset -= _offset % buffer_offset_alignment;
		}

		void GLUniformAdapter::bind(const UniformData & data)
		{
			glBindBufferRange(GL_UNIFORM_BUFFER, _index, _buffers[data.buffer].handle, data.offset, _size);
		}
	}
}

//---------------------------------------------------------------------------
