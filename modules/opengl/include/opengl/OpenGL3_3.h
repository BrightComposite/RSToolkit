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
		class GLModel;
	}

	link_class(OpenGL3_3::GraphicContext, Class<Graphics3D>);
	link_class(OpenGL3_3::Graphics3D, Class<OpenGL3_3::GraphicContext>);
	link_class(OpenGL3_3::GLModel, Class<Model>);

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

		class GLImage : public Image
		{
			friend class Graphics3D;

		public:
			GLImage(const GraphicContext * ctx, const ImageData & data);

			virtual void apply() const override;
			virtual void requestData(ImageData * output) const override;

		protected:
			const GraphicContext * _ctx;
			float _scale[2];
			uint  _id;
		};

		typedef GLImage GLTexture;

		//---------------------------------------------------------------------------

		class VertexElement : public Shared
		{
			friend class VertexLayout;

		public:
			string id;
			byte location;
			uint size;

			static VertexElement pos2;
			static VertexElement pos3;
			static VertexElement color3;
			static VertexElement color4;
			static VertexElement secondaryColor3;
			static VertexElement secondaryColor4;
			static VertexElement tex;
			static VertexElement normal;

		protected:
			VertexElement(const string & id, byte location, uint size) : Precached<string, VertexElement>(id),
				id(id), location(location), size(size) {}
		};

		//---------------------------------------------------------------------------

		class VertexLayout : public Shared
		{
			typedef Cached<string, VertexLayout, ProtectedCache> Cache;

		public:
			string key;
			Array<VertexElement> elements;
			uint stride;

		protected:
			public_for_cache(Cache);

			VertexLayout(const String & key) : key(key)
			{
				stride = decodeData(key, elements);
			}

			static uint decodeData(const String & data, Array<VertexElement> & elements)
			{
				uint stride = 0;
				auto list = split(data);

				for(const auto & str : *list)
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

		//---------------------------------------------------------------------------

		class UniformBase : public Shared
		{
			friend class Graphics3D;

		public:
			UniformBase(ShaderType shader, uint index, uint size);

			void update(const void * data);

		protected:
			uint index;
			GraphicContext * ctx;
		};

		template<class T>
		class Uniform : public UniformBase, public StaticIdentifier<Uniform<T>>, public HandleSingleton<Uniform<T>>
		{
		public:
			Uniform() : UniformBase(T::shader, T::index, static_cast<uint>(sizeof(T))) {}
		};

		using UniformId = PointerId;

		class UniformData
		{
		public:
			UniformData(UniformBase * uniform) : uniform(uniform) {}

			template<class T,
				useif(
					is_uniform<T>::value
					)>
			void set(const T & data)
			{
				uniform->update(&data);
			}

		protected:
			UniformBase * uniform;
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

		template<ShaderType Type>
		class Shader {};

		typedef Shader<ShaderType::Common>		CommonShader;
		typedef Shader<ShaderType::Vertex>		VertexShader;
		typedef Shader<ShaderType::Fragment>	FragmentShader;
		typedef Shader<ShaderType::Pixel>		PixelShader;

		typedef RawData<const void> ShaderCode;
		typedef Map<ShaderType, ShaderCode> ShaderCodeSet;
		typedef Map<string, ShaderCodeSet> ShaderMap;

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

		class Graphics3D : public GraphicContext, public Singleton<Handle<Graphics3D, Graphics3D>, ThreadLocalModel>
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

			static inline Handle<Graphics3D, Graphics3D> & findInstance(const thread::id & id);

			static inline Graphics3D * initialize();
			static inline Graphics3D * initialize(const Handle<Graphics3D> & inst);
			static inline void free();

			virtual void clip(const IntRect & rect) override;
			virtual void rectangle(const IntRect & rect) override;
			virtual void ellipse(const IntRect & rect) override;
			virtual void rectangle(const SqRect & r) override;
			virtual void ellipse(const SqRect & r) override;

			virtual void draw(const Figure * figure, const IntRect & bounds) override;
			virtual void draw(const Figure * figure, const FloatTransform & transform) override;

			virtual void draw(const Model * model) override;

			virtual void draw(const Image * image, int x, int y) override;
			virtual void draw(const Image * image, int x, int y, int width, int height) override;
			virtual void draw(const Image * image, const IntRect & rect) override;
			virtual void draw(const Image * image, const SqRect & r) override;

			virtual void draw(const Symbol * symbol, int x, int y) override;

			virtual void present() const override;

			virtual void printInfo();
			virtual void printDebug();
			virtual void checkForErrors();

			virtual void bind(const Handle<Surface> & surface) override;
			virtual void bind(const Handle<Texture> & texture, uint index) override;

			virtual Handle<Image> createImage(const ImageData & data) const override;
			virtual Handle<Figure> createFigure(const FigureData & data) const override;

			template<class T, useif(is_uniform<T>::value)>
			void updateUniform(const T & uniform) const;
			template<class T, typename ... A, useif(is_uniform<T>::value && can_construct(T, A...))>
			void updateUniform(A && ... args) const;

			static Class<Subject> meta;

			static Map<thread::id, Graphics3D, Graphics3D> pool;

		protected:
			public_for_handle(Graphics3D, Graphics3D);
			friend class GLModel;

			Graphics3D();
			virtual ~Graphics3D();

			virtual Handle<Surface> createWindowSurface(WindowAdapter * adapter) const override;
			virtual void updateBrushState() override;

			template<class T, typename ... Components, typename ... A>
			void setUniform(Type<Uniforms::Base<Components...>> type, Handle<UniformData> & data, A && ... args) const
			{
				data->set(T {static_cast<adapt_t<Components, A>>(forward<A>(args))...});
			}

			void draw(const GLModel * model);

			void initDevice();
			void initFacilities();
			void freeFacilities();

			static inline void updatePool();

			Handle<VertexShader> vshader = nullptr;
			Handle<PixelShader> pshader = nullptr;
			
			mutable Handle<VertexBuffer> vbuffer = nullptr;
			mutable Map<UniformId, UniformData> uniformData;

			Array<Texture> textures;
		};

		inline Handle<Graphics3D, Graphics3D> & Graphics3D::findInstance(const thread::id & id)
		{
			return pool[id];
		}

		inline Graphics3D * Graphics3D::initialize()
		{
			auto & graphics = instance().init();

			graphics->initDevice();
			graphics->initFacilities();

			updatePool();

			return graphics;
		}

		inline Graphics3D * Graphics3D::initialize(const Handle<Graphics3D> & inst)
		{
			auto & graphics = instance();

			if(graphics == inst)
				return graphics;

			graphics = inst;
			updatePool();

			return graphics;
		}

		inline void Graphics3D::free()
		{
			auto & graphics = instance();

			if(graphics == nullptr)
				return;

			graphics->freeFacilities();
			graphics = nullptr;
			updatePool();
		}

		inline void Graphics3D::updatePool()
		{
			pool[std::this_thread::get_id()] = instance();
		}

		template<class T, selectif_t<0>>
		void Graphics3D::updateUniform(const T & uniform) const
		{
			Handle<UniformData> & ud = uniformData[Uniform<T>::id()];

			if(ud == nullptr)
				ud.init(Uniform<T>::instance());

			ud->set(uniform);
		}

		template<class T, typename ... A, selectif_t<1>>
		void Graphics3D::updateUniform(A && ... args) const
		{
			Handle<UniformData> & ud = uniformData[Uniform<T>::id()];

			if(ud == nullptr)
				ud.init(Uniform<T>::instance());

			setUniform<T>(gettype(typename T::Base), ud, forward<A>(args)...);
		}

		//---------------------------------------------------------------------------

		class Shaders
		{
			friend class Graphics3D;

		public:
			static ShaderCode getCode(const string & id, ShaderType type)
			{
				auto & codeSet = shaders[id];

				if(codeSet.isNull())
					throw new Exception("Can't find embedded shader set with id \"", id, "\"");

				auto & code = (*codeSet)[type];

				if(code.isNull())
					throw new Exception("Embedded shader set with id \"", id, "\" doesn't contain shader of type \"", type, "\"");

				return *code;
			}

			static Handle<ShaderCodeSet> & setCode(const string & id, const initializer_list<ShaderCode> & codeSet)
			{
				auto & set = shaders[id].reinit();

				for(size_t i = 0; i < codeSet.size(); ++i)
					set->insert_or_assign(static_cast<ShaderType>(i), handle<ShaderCode>(*(codeSet.begin() + i)));

				return set;
			}

		protected:
			static void initialize();
			static void free();

			static ShaderMap shaders;
		};

		//---------------------------------------------------------------------------

		class GLModel : public Model
		{
		public:
			GLModel(const Graphics3D * graphics, const Handle<VertexLayout> & vil, const VertexData & vertexData)
				: Model(graphics, vertexData, vil->stride), buffer(vil, vertexData)
			{
				setclass(GLModel);
			}

			Handle<VertexBuffer> buffer;

			static Handle<GLModel, Graphics3D> quad;
			static Handle<GLModel, Graphics3D> texquad;
		};

		class GLFigure : public Figure
		{
		public:
			GLFigure(const Graphics3D * graphics, const FigureData & data);

			Handle<GLModel> model;
		};

		//---------------------------------------------------------------------------

		class GLSurface : public Surface
		{
			friend class Graphics3D;

		public:
			GLSurface(const IntSize & size);
			virtual ~GLSurface();

			virtual void clear() const;

		protected:
			GraphicContext * ctx;
			void createDepthStencil();

			RC _renderCtx;
			Graphics3D::ContextAttributes attr;
		};

		class DepthBufferSurface : public GLSurface
		{
		public:
			DepthBufferSurface(const IntSize & size);
			DepthBufferSurface(const DepthBufferSurface &) = delete;
			virtual ~DepthBufferSurface() {}

			virtual void apply() const override;
		};

		class WindowSurface : public GLSurface
		{
			friend class Graphics3D;

		public:
			WindowSurface(WindowAdapter * adapter);
			WindowSurface(const WindowSurface &) = delete;
			virtual ~WindowSurface();

			virtual void apply() const override;
			virtual void present() const override;
			virtual void requestData(ImageData * data) const override;

		protected:
			void createSwapChain(bool fullscreen);
			void createRenderTarget();
			void releaseRenderTarget();

			static void findPreferredMode(const ComHandle<IDXGIOutput> & output, DXGI_MODE_DESC & mode);

			void onWindowResize(Handle<WindowResizeMessage> & msg, WindowAdapter & adapter, const Subject * src);
			void onWindowFullscreen(Handle<WindowFullscreenMessage> & msg, WindowAdapter & adapter, const Subject * src);
			void onWindowClose(Handle<WindowCloseMessage> & msg, WindowAdapter & adapter, const Subject * src);

			ComHandle<IDXGISwapChain1> swapChain;
			ComHandle<ID3D11RenderTargetView> renderTargetView;

			DXGI_PRESENT_PARAMETERS presentParams;
			WindowAdapter * adapter;
		};

		class TextureSurface : public GLSurface
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

			template<class ShaderProgramType, class ... A, useif(can_construct(ShaderProgramType, Handle<FxTechnique>, A...))>
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
