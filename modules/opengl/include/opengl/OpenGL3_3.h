//---------------------------------------------------------------------------

#ifndef OPENGL_3_3_H
#define OPENGL_3_3_H

//---------------------------------------------------------------------------

#include <graphics/Graphics3D.h>

//---------------------------------------------------------------------------

#include <gl/glew.h>

#ifdef _WIN32
#include <gl/wglew.h>
#else
#include <gl/glxew.h>
#endif // _WIN32

#include <core/Subject.h>
#include <core/addition/Cached.h>
#include <core/addition/HandleSingleton.h>
#include <core/container/Array.h>

#include <ui/WidgetMessages.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::thread;

	namespace OpenGL3_3
	{
		class GraphicContext;
		class Graphics3D;
		class Model;
		class Image;
		class Surface;
	}

	link_class(OpenGL3_3::GraphicContext, Class<Graphics3D>);
	link_class(OpenGL3_3::Graphics3D, Class<OpenGL3_3::GraphicContext>);
	link_class(OpenGL3_3::Model, Class<Model>);

	namespace OpenGL3_3
	{
	#ifdef _WIN32
		typedef HGLRC RC;
	#else
		typedef GLXContext RC;
	#endif // _WIN32

		//---------------------------------------------------------------------------

		class GraphicContext : public Rapture::Graphics3D
		{
		public:
			HGLRC _handle;

		protected:
			HWND _wnd;
			HDC _device;
		};

		//---------------------------------------------------------------------------

		class Image : public Rapture::Image
		{
			friend class Graphics3D;

		public:
			Image(const GraphicContext * ctx, const ImageData & data);

			virtual void apply() const override;
			virtual void requestData(ImageData * output) const override;

		protected:
			const GraphicContext * _ctx;
			float _scale[2];
			uint  _id;
		};

		typedef Image Texture;

		//---------------------------------------------------------------------------

		class VertexElement : public Shared, public Precached<string, VertexElement>
		{
			friend class VertexLayout;

		public:
			string id;
			byte location;
			uint size;

			static VertexElement pos2;
			static VertexElement pos3;
			static VertexElement color3;
			static VertexElement colorf;
			static VertexElement secondaryColor3;
			static VertexElement secondaryColor4;
			static VertexElement tex;
			static VertexElement normal;

		protected:
			VertexElement(const string & id, byte location, uint size) : Precached<string, VertexElement>(id),
				id(id), location(location), size(size) {}
		};

		//---------------------------------------------------------------------------

		class VertexLayout : public Shared, public Cached<string, VertexLayout>
		{
			typedef Cached<string, VertexLayout> Cache;

		public:
			string key;
			Array<VertexElement> elements;
			uint stride;

		protected:
			friend_handle;

			VertexLayout(const String & key) : key(key)
			{
				stride = decodeData(key, elements);
			}

			static uint decodeData(const String & data, Array<VertexElement> & elements)
			{
				uint stride = 0;

				for(const auto & str : split(data))
				{
					auto & vie = VertexElement::get(str);
					elements.push_back(vie);
					stride += vie->size;
				}

				return stride;
			}
		};

		// TODO: save current layout in Graphics and apply it with the glVertexAttribPointer and the glEnableVertexAttribArray functions

		//---------------------------------------------------------------------------

		class VertexBuffer : public Shared
		{
		public:
			VertexBuffer(const Handle<VertexLayout> & vil, const VertexData & vd, uint topology = GL_TRIANGLES);

			GraphicContext * ctx;
			Handle<VertexLayout> vil;
			uint handle;
			uint topology;
			uint size;
		};

		class IndexBuffer : public Shared
		{
		public:
			IndexBuffer(const VertexIndices & indices);

			GraphicContext * ctx;
			uint handle;
			uint size;
		};

		//---------------------------------------------------------------------------

		class Uniform : public Shared
		{
			friend class Graphics3D;

		public:
			Uniform(ShaderType shader, uint index, uint size);

		protected:
			void update(const void * data);

			uint buffer;
			uint index;
			GraphicContext * ctx;
		};

		template<class T, bool isValid = is_uniform<T>::value>
		class GenericUniform {};

		template<class T>
		class GenericUniform<T, true> : public Uniform, public StaticIdentifier<GenericUniform<T>>, public Singleton<GenericUniform<T>, ThreadLocalModel>
		{
		public:
			GenericUniform() : Uniform(T::shader, T::index, static_cast<uint>(sizeof(T))) {}

			void set(const T & data)
			{
				update(&data);
			}

			template<class ... A, useif <can_construct<T, A...>::value> endif>
			void set(A &&... args)
			{
				T data(forward<A>(args)...);
				update(&data);
			}
		};

		//---------------------------------------------------------------------------

		class FxTechnique : public Shared
		{
			friend class Graphics3D;
			friend class VertexBuffer;

			template<ShaderType>
			friend class Shader;

		public:
			FxTechnique() {}
			virtual ~FxTechnique() {}

			virtual void apply(uint pass = 0) const = 0;
			
			const Handle<VertexLayout> & layout() const
			{
				return vil;
			}

		protected:
			FxTechnique(const Handle<VertexLayout> & vil);

			GraphicContext * ctx;
			Handle<VertexLayout> vil;
			uint passes;
		};

		//---------------------------------------------------------------------------

		template<ShaderType type>
		class Shader {};

		typedef Shader<ShaderType::Common>		CommonShader;
		typedef Shader<ShaderType::Vertex>		VertexShader;
		typedef Shader<ShaderType::Fragment>	FragmentShader;
		typedef Shader<ShaderType::Pixel>		PixelShader;

		typedef RawData<const void> ShaderCode;
		typedef HashMap<ShaderType, ShaderCode> ShaderCodeSet;
		typedef HashMap<string, ShaderCodeSet> ShaderMap;

		enum class ShaderCodeState
		{
			Raw,
			Compiled,
			Embedded
		};

		template<>
		class Shader<ShaderType::Common> : public Shared
		{
		public:
			Shader();
			virtual void apply() const = 0;

		protected:
			void read(const String & filename, ShaderCode & out);
			void compile(const String & filename, const String & entrance, const String & shaderModel, ShaderCode & out);
		
			GraphicContext * ctx;
		};

		template<>
		class Shader<ShaderType::Vertex> : public CommonShader
		{
			void init(const Handle<FxTechnique> & technique, const ShaderCode & code);

		public:
			Shader(const Handle<FxTechnique> & technique, const String & path, ShaderCodeState state = ShaderCodeState::Compiled);
			Shader(const Handle<FxTechnique> & technique, const Handle<ShaderCode> & code);

			virtual void apply() const override;

			uint id;
		};

		template<>
		class Shader<ShaderType::Pixel> : public CommonShader
		{
			void init(const Handle<FxTechnique> & technique, const ShaderCode & code);

		public:
			Shader(const Handle<FxTechnique> & technique, const String & path, ShaderCodeState state = ShaderCodeState::Compiled);
			Shader(const Handle<FxTechnique> & technique, const Handle<ShaderCode> & code);

			virtual void apply() const override;

			uint id;
		};

		//---------------------------------------------------------------------------

		class Graphics3D : public GraphicContext
		{
		public:
			struct ContextAttributes
			{
				ContextAttributes(int major, int minor, int flags) : major(major), minor(minor), flags(flags) {}
				ContextAttributes() : ContextAttributes(0, 0, 0) {}
				ContextAttributes(const ContextAttributes & a) : ContextAttributes(a.major, a.minor, a.flags) {}

				int major;
				int minor;
				int flags;
			};

			friend Handle<Graphics3D, Graphics3D> provide();

			virtual void clip(const IntRect & rect) override;
			virtual void rectangle(const IntRect & rect) override;
			virtual void ellipse(const IntRect & rect) override;
			virtual void rectangle(const SqRect & r) override;
			virtual void ellipse(const SqRect & r) override;

			virtual void draw(const Rapture::Figure * figure, const IntRect & bounds) override;
			virtual void draw(const Rapture::Figure * figure, const FloatTransform & transform) override;

			virtual void draw(const Rapture::Model * model) override;

			virtual void draw(const Rapture::Image * image, int x, int y) override;
			virtual void draw(const Rapture::Image * image, int x, int y, int width, int height) override;
			virtual void draw(const Rapture::Image * image, const IntRect & rect) override;
			virtual void draw(const Rapture::Image * image, const SqRect & r) override;

			virtual void draw(const Rapture::Symbol * symbol, int x, int y) override;

			virtual void present() const override;

			virtual void printInfo();
			virtual void printDebug();
			virtual void checkForErrors();

			using Rapture::Graphics::bind;

			virtual void bind(const Handle<Rapture::Surface> & surface) override;
			virtual void bind(const Handle<Rapture::Texture> & texture, uint index) override;

			virtual Handle<Rapture::Image> createImage(const ImageData & data) override;
			virtual Handle<Rapture::Figure> createFigure(const FigureData & data) override;

		protected:
			friend_handle;
			friend class Model;

			Graphics3D();
			virtual ~Graphics3D();

			virtual Handle<Rapture::Surface> createSurface(UISpace * space) override;
			virtual void updateBrushState() override;

			void draw(const Model * model);

			void initDevice();
			void initFacilities();
			void freeFacilities();

			Handle<VertexShader> vshader = nullptr;
			Handle<PixelShader> pshader = nullptr;

			mutable Handle<VertexBuffer> vbuffer = nullptr;
			mutable Handle<IndexBuffer> ibuffer = nullptr;
			mutable TypedSet<Uniform> uniformData;

			Array<Texture> textures;
		};

		inline Handle<Graphics3D, Graphics3D> provide()
		{
			return {emptiness};
		}

		//---------------------------------------------------------------------------

		class Shaders
		{
			friend class Graphics3D;

		public:
			static ShaderCode getCode(const string & id, ShaderType type)
			{
				auto i = shaders.find(id);

				if(i == shaders.end())
					throw Exception("Can't find embedded shader set with id \"", id, "\"");

				auto & set = i->second;
				auto ci = set->code.find(type);

				if(ci == set->code.end())
					throw Exception("Embedded shader set with id \"", id, "\" doesn't contain shader of type \"", type, "\"");

				return *ci->second;
			}

			static Handle<ShaderCodeSet> setCode(const string & id, const initializer_list<Handle<ShaderCode>> & codeSet)
			{
				Handle<ShaderCodeSet> set;
				auto i = shaders.find(id);

				if(i == shaders.end())
					shaders.insert({id, set.init()});
				else
					set = i->second;

				for(size_t i = 0; i < codeSet.size(); ++i)
					set->code.insert({static_cast<ShaderType>(i), *(codeSet.begin() + i)});

				return set;
			}

		protected:
			static void initialize();
			static void free();

			static ShaderMap shaders;
		};

		//---------------------------------------------------------------------------

		class Model : public Rapture::Model
		{
		public:
			Model(const Graphics3D * graphics, const Handle<VertexLayout> & vil, const VertexData & vertexData)
				: Rapture::Model(graphics, vertexData, vil->stride), buffer(vil, vertexData)
			{
				setclass(Model);
			}

			Handle<VertexBuffer> buffer;

			static Handle<Model, Graphics3D> quad;
			static Handle<Model, Graphics3D> texquad;
		};

		class Figure : public Rapture::Figure
		{
		public:
			Figure(const Graphics3D * graphics, const FigureData & data);

			Handle<Model> model;
		};

		//---------------------------------------------------------------------------

		class Surface : public Rapture::Surface
		{
			friend class Graphics3D;

		public:
			Surface(const IntSize & size);
			virtual ~Surface();

			virtual void clear() const;

		protected:
			GraphicContext * ctx;
			void createDepthStencil();

			RC _renderCtx;
			Graphics3D::ContextAttributes attr;
		};

		class DepthBufferSurface : public Surface
		{
		public:
			DepthBufferSurface(const IntSize & size);
			DepthBufferSurface(const DepthBufferSurface &) = delete;
			virtual ~DepthBufferSurface() {}

			virtual void apply() const override;
		};

		class UISurface : public Surface
		{
			friend class Graphics3D;

		public:
			UISurface(UISpace * space);
			UISurface(const UISurface &) = delete;
			virtual ~UISurface();

			virtual void apply() const override;
			virtual void present() const override;
			virtual void requestData(ImageData * data) const override;

		protected:
			void createSwapChain(bool fullscreen);
			void createRenderTarget();
			void releaseRenderTarget();

			void onUIResize(Handle<UIResizeMessage> & msg, UISpace & space, const Subject * src);
			void onUIFullscreen(Handle<UIFullscreenMessage> & msg, UISpace & space, const Subject * src);
			void onUIDestroy(Handle<WindowCloseMessage> & msg, UISpace & space, const Subject * src);

			UISpace * space;
		};

		class TextureSurface : public Surface
		{
		public:
			TextureSurface(const IntSize & size);
			virtual ~TextureSurface() {}

			virtual void apply() const override;
		};

		//---------------------------------------------------------------------------

		class ShaderProgram : public Shared
		{
		public:
			virtual ~ShaderProgram() {}
			virtual void apply() const {}

		protected:
			ShaderProgram() {}
			uint id;
		};

		class VPShaderProgram : public ShaderProgram
		{
		public:
			VPShaderProgram(const Handle<FxTechnique> & technique, const string & filename, ShaderCodeState state = ShaderCodeState::Compiled) : vs(technique, filename, state), ps(technique, filename, state) {}
			VPShaderProgram(const Handle<FxTechnique> & technique, const Handle<ShaderCodeSet> & codeSet) : vs(technique, codeSet->at(ShaderType::Vertex)), ps(technique, codeSet->at(ShaderType::Pixel)) {}
			virtual ~VPShaderProgram() {}

			virtual void apply() const override
			{
				vs->apply();
				ps->apply();
			}

		protected:
			Handle<VertexShader> vs;
			Handle<PixelShader> ps;
		};

		//---------------------------------------------------------------------------

		class SimpleTechnique : public FxTechnique
		{
		public:
			SimpleTechnique(const Handle<VertexLayout> & vil)
				: FxTechnique(vil), program() {}

			SimpleTechnique(const Handle<VertexLayout> & vil, const Handle<ShaderProgram> & program)
				: FxTechnique(vil), program(program) {}

			template<class ShaderProgramType, class ... A, useif <can_construct<ShaderProgramType, Handle<FxTechnique>, A...>::value> endif>
			SimpleTechnique(const Handle<VertexLayout> & vil, const Type<ShaderProgramType> &, A &&... args)
				: FxTechnique(vil), program(handle<ShaderProgramType>(this, forward<A>(args)...)) {}

			virtual ~SimpleTechnique() {}

			void bind(const Handle<ShaderProgram> & program)
			{
				this->program = program;
			}

			virtual void apply(uint pass = 0) const override
			{
				program->apply();
			}

			static Handle<SimpleTechnique, Shaders> rectangle;
			static Handle<SimpleTechnique, Shaders> ellipse;
			static Handle<SimpleTechnique, Shaders> wired_rectangle;
			static Handle<SimpleTechnique, Shaders> wired_ellipse;
			static Handle<SimpleTechnique, Shaders> figure;
			static Handle<SimpleTechnique, Shaders> image;
			static Handle<SimpleTechnique, Shaders> text;

		protected:
			Handle<ShaderProgram> program;
		};
	}

#ifndef OpenGL
#define OpenGL OpenGL3_3
	using GLGraphics = OpenGL3_3::Graphics3D;
#endif
}

//---------------------------------------------------------------------------
#endif
