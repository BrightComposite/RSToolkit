//---------------------------------------------------------------------------

#ifndef DIRECT3D11_H
#define DIRECT3D11_H

//---------------------------------------------------------------------------

#ifndef _MSC_VER
#error "You can use DirectX renderer only with MSVS"
#else

//---------------------------------------------------------------------------

#include <graphics/Graphics3D.h>

#include <d3d11_2.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>

#include <core/Subject.h>
#include <core/addition/Cached.h>
#include <core/container/TypedMap.h>
#include <core/addition/HandleSingleton.h>
#include <core/container/Array.h>

#include <windows/ComHandle.h>
#include <windows/ComException.h>

#include <ui/WidgetMessages.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::thread;

	namespace Direct3D11
	{
		class GraphicContext;
		class Graphics3D;
		class D3DModel;
		class D3DIndexedModel;
	}

	link_class(Direct3D11::GraphicContext, Class<Graphics3D>);
	link_class(Direct3D11::Graphics3D, Class<Direct3D11::GraphicContext>);
	link_class(Direct3D11::D3DModel, Class<Model>);
	link_class(Direct3D11::D3DIndexedModel, Class<Direct3D11::D3DModel>);

	namespace Direct3D11
	{
		class GraphicContext : public Rapture::Graphics3D
		{
		public:
			ComHandle<ID3D11Device2, GraphicContext> device;
			ComHandle<ID3D11DeviceContext2, GraphicContext> context;
			ComHandle<ID3D11RasterizerState, GraphicContext> solidRS;
			ComHandle<ID3D11RasterizerState, GraphicContext> wiredRS;
			ComHandle<ID3D11DepthStencilState, GraphicContext> depthState2D;
			ComHandle<ID3D11DepthStencilState, GraphicContext> depthState3D;
			ComHandle<ID3D11BlendState, GraphicContext> blendState;
			ComHandle<ID3D11SamplerState, GraphicContext> linearSampler;
			ComHandle<IDXGIFactory2, GraphicContext> dxgiFactory;
			ComHandle<ID3D11Debug> debug;

		protected:
			inline void setContext(const ComHandle<ID3D11Device1> & device, const ComHandle<ID3D11DeviceContext2> & context)
			{
				this->device = device;
				this->context = context;
			}
		};

//---------------------------------------------------------------------------

		class D3DImage : public Image
		{
			friend class Graphics3D;

		public:
			D3DImage(GraphicContext * ctx, const ImageData & data);
			virtual ~D3DImage() {}

			virtual void apply() const override;
			virtual void requestData(ImageData * output) const override;

		protected:
			ComHandle<ID3D11SamplerState> _state;
			ComHandle<ID3D11ShaderResourceView> _handle;
			GraphicContext * _ctx;
		};

		typedef D3DImage D3DTexture;

//---------------------------------------------------------------------------

		class VertexElement : public Shared, public Precached<string, VertexElement>
		{
			friend class VertexLayout;

		public:
			string id;
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
			VertexElement(const string & id, const char * semantic, uint index, DXGI_FORMAT format, uint size) : Precached<string, VertexElement>(id),
				id(id), semantic(semantic), index(index), format(format), size(size) {}

			const char * semantic;
			uint index;
			DXGI_FORMAT format;
		};

//---------------------------------------------------------------------------

		class VertexLayout : public Shared, public Cached<string, VertexLayout>
		{
			typedef Cached<string, VertexLayout> Cache;

		public:
			string key;
			Array<VertexElement> elements;
			uint stride;
			vector<D3D11_INPUT_ELEMENT_DESC> layout;

		protected:
			friend_owned_handle(VertexLayout, Cache);

			VertexLayout(const string & key) : key(key)
			{
				stride = decodeData(key, elements, layout);
			}

			static uint decodeData(const string & data, Array<VertexElement> & elements, vector<D3D11_INPUT_ELEMENT_DESC> & layout)
			{
				uint stride = 0;

				for(const auto & str : split(data))
				{
					auto & vie = VertexElement::get(str);
					elements.push_back(vie);
					layout.push_back({vie->semantic, vie->index, vie->format, 0, stride, D3D11_INPUT_PER_VERTEX_DATA, 0});
					stride += vie->size;
				}

				return stride;
			}
		};

//---------------------------------------------------------------------------

		class VertexBuffer : public Shared
		{
		public:
			VertexBuffer(GraphicContext * ctx, const Handle<VertexLayout> & vil, const VertexData & vd, D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			GraphicContext * ctx;
			D3D_PRIMITIVE_TOPOLOGY topology;
			Handle<VertexLayout> vil;
			ComHandle<ID3D11Buffer> handle;
			uint size;
		};

		class IndexBuffer : public Shared
		{
		public:
			IndexBuffer(GraphicContext * ctx, const VertexIndices & indices);

			GraphicContext * ctx;
			ComHandle<ID3D11Buffer> handle;
			uint size;
		};

//---------------------------------------------------------------------------
		
		class UniformAdapter : public Rapture::UniformAdapter
		{
			friend class Graphics3D;

		public:
			UniformAdapter(GraphicContext * ctx, ShaderType shader, int index, size_t size);

		protected:
			virtual void update(const void * data) override;

			ComHandle<ID3D11Buffer> buffer;
			int index;
			GraphicContext * ctx;
		};

//---------------------------------------------------------------------------

		class FxTechnique : public Shared
		{
			friend class Graphics3D;
			friend class VertexBuffer;

			template<ShaderType>
			friend class Shader;

		public:
			virtual ~FxTechnique() {}

			virtual void apply(uint pass = 0) const = 0;
			
			const Handle<VertexLayout> & layout() const
			{
				return vil;
			}

		protected:
			FxTechnique(GraphicContext * ctx, const Handle<VertexLayout> & vil);

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
			Shader(GraphicContext * ctx);
			virtual void apply() const = 0;

		protected:
			void read(const String & filename, Handle<ShaderCode> & out);
			void compile(const String & filename, const String & entrance, const String & shaderModel, Handle<ShaderCode> & out);
		
			GraphicContext * ctx;
		};

		template<>
		class Shader<ShaderType::Vertex> : public CommonShader
		{
			void init(const Handle<FxTechnique> & technique, const Handle<ShaderCode> & code);

		public:
			Shader(const Handle<FxTechnique> & technique, const String & path, ShaderCodeState state = ShaderCodeState::Compiled);
			Shader(const Handle<FxTechnique> & technique, const Handle<ShaderCode> & code);

			virtual void apply() const override;

			ComHandle<ID3D11VertexShader> id;
			ComHandle<ID3D11InputLayout> layout;
		};

		template<>
		class Shader<ShaderType::Pixel> : public CommonShader
		{
			void init(const Handle<FxTechnique> & technique, const Handle<ShaderCode> & code);

		public:
			Shader(const Handle<FxTechnique> & technique, const String & path, ShaderCodeState state = ShaderCodeState::Compiled);
			Shader(const Handle<FxTechnique> & technique, const Handle<ShaderCode> & code);

			virtual void apply() const override;

			ComHandle<ID3D11PixelShader> id;
		};

		class ShaderProgram : public Shared
		{
		public:
			virtual ~ShaderProgram() {}
			virtual void apply() const {}

		protected:
			ShaderProgram() {}
		};

		template<class T>
		using is_shader_program = is_base_of<ShaderProgram, T>;

		//---------------------------------------------------------------------------

		class SimpleTechnique : public FxTechnique
		{
		public:
			SimpleTechnique(GraphicContext * ctx, const Handle<VertexLayout> & vil)
				: FxTechnique(ctx, vil), program() {}

			SimpleTechnique(GraphicContext * ctx, const Handle<VertexLayout> & vil, const Handle<ShaderProgram> & program)
				: FxTechnique(ctx, vil), program(program) {}

			template<class Program, class ... A, useif <
				is_shader_program<Program>::value,
				can_construct<Program, Handle<FxTechnique>, A...>::value
				> endif
			>
			SimpleTechnique(GraphicContext * ctx, const Handle<VertexLayout> & vil, const Type<Program> &, A &&... args)
				: FxTechnique(ctx, vil), program(handle<Program>(this, forward<A>(args)...)) {}

			virtual ~SimpleTechnique() {}

			void bind(const Handle<ShaderProgram> & program)
			{
				this->program = program;
			}

			virtual void apply(uint pass = 0) const override
			{
				program->apply();
			}

		protected:
			Handle<ShaderProgram> program;
		};

		//---------------------------------------------------------------------------

		class Shaders : public Shared
		{
			friend class Graphics3D;
			friend_handle;

		public:
			const Handle<ShaderCode> & getCode(const string & id, ShaderType type) const
			{
				auto i = shaders.find(id);

				if(i == shaders.end())
					throw Exception("Can't find embedded shader set with id \"", id, "\"");

				auto & set = i->second;
				auto ci = set->code.find(type);

				if(ci == set->code.end())
					throw Exception("Embedded shader set with id \"", id, "\" doesn't contain shader of type \"", type, "\"");

				return ci->second;
			}

			Handle<ShaderCodeSet> & setCode(const string & id, const initializer_list<Handle<ShaderCode>> & codeSet)
			{
				auto i = shaders.find(id);

				if(i == shaders.end())
					i = shaders.insert(i, {id, handle<ShaderCodeSet>()});

				auto & set = i->second;

				for(size_t i = 0; i < codeSet.size(); ++i)
					set->code.insert({static_cast<ShaderType>(i), *(codeSet.begin() + i)});

				return set;
			}

			Handle<SimpleTechnique, Shaders> rectangle;
			Handle<SimpleTechnique, Shaders> ellipse;
			Handle<SimpleTechnique, Shaders> wired_rectangle;
			Handle<SimpleTechnique, Shaders> wired_ellipse;
			Handle<SimpleTechnique, Shaders> figure;
			Handle<SimpleTechnique, Shaders> image;
			Handle<SimpleTechnique, Shaders> text;

		protected:
			Shaders(GraphicContext * ctx);

			template<class ShaderType, class ... A>
			Handle<SimpleTechnique> setCode(const string & id, const Handle<VertexLayout> & vil, ShaderType type, A &&... codeData)
			{
				return handle<SimpleTechnique>(ctx, vil, type, setCode(id, {handle<ShaderCode>(forward<A>(codeData))...}));
			}

			ShaderMap shaders;
			GraphicContext * ctx;
		};

		//---------------------------------------------------------------------------

		class D3DModel : public Model
		{
		public:
			D3DModel(GraphicContext * ctx, const Handle<VertexLayout> & vil, const VertexData & vertexData)
				: Model(ctx, vertexData, vil->stride), buffer(ctx, vil, vertexData)
			{
				setclass(D3DModel);
			}

			Handle<VertexBuffer> buffer;
		};

		class D3DIndexedModel : public D3DModel
		{
		public:
			D3DIndexedModel(GraphicContext * ctx, const Handle<VertexLayout> & vil, const VertexData & vertexData, const VertexIndices & indices)
				: D3DModel(ctx, vil, vertexData), indices(ctx, indices)
			{
				setclass(D3DIndexedModel);
			}

			Handle<IndexBuffer> indices;
			uint indicesLocation;
		};

		class D3DFigure : public Figure
		{
		public:
			D3DFigure(GraphicContext * graphics, const FigureData & data);

			Handle<D3DModel> model;
		};

		//---------------------------------------------------------------------------

		class Graphics3D : public GraphicContext
		{
		public:
			friend inline Handle<Graphics3D> provide();

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

			virtual void draw(const Symbol * symbol, int x, int y) override;

			virtual void present() const override;

			virtual void printInfo();
			virtual void printDebug();
			virtual void checkForErrors();

			using GraphicContext::bind;

			virtual void bind(const Handle<Rapture::Surface> & surface) override;
			virtual void bind(const Handle<Rapture::Texture> & texture, uint index) override;

			virtual Handle<Rapture::Image> createImage(const ImageData & data) override;
			virtual Handle<Rapture::Figure> createFigure(const FigureData & data) override;

			virtual Handle<ShaderCode> getShaderCode(const string & id, ShaderType type) override;

			Handle<D3DModel, Graphics3D> quad;
			Handle<D3DModel, Graphics3D> texquad;

		protected:
			friend_handle;
			friend class D3DModel;
			friend class TypedMap<Uniforms::Base, Uniform>;

			Graphics3D();
			virtual ~Graphics3D();

			virtual Handle<Rapture::Surface> createSurface(UISpace * space) override;
			virtual Handle<Rapture::UniformAdapter> createUniformAdapter(ShaderType shader, int index, size_t size) override;

			virtual void updateBrushState() override;

			void draw(const D3DModel * model);

			void initDevice();
			void initFacilities();
			void freeFacilities();

			Handle<VertexShader> vshader = nullptr;
			Handle<PixelShader> pshader = nullptr;
			
			Handle<Shaders> shaders = nullptr;

			Handle<VertexBuffer> vbuffer = nullptr;
			Handle<IndexBuffer> ibuffer = nullptr;

			Array<Texture> textures;

			D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
			D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
		};

		inline Handle<Graphics3D> provide()
		{
			return {emptiness};
		}

		//---------------------------------------------------------------------------

		class D3DSurface : public Surface
		{
			friend class Graphics3D;

		public:
			D3DSurface(Graphics3D * graphics, const IntSize & size);
			virtual ~D3DSurface();

			virtual void clear() const;

		protected:
			Graphics3D * graphics;
			void createDepthStencil();

			ComHandle<ID3D11DepthStencilView> depthStencilView;
		};

		class DepthBufferSurface : public D3DSurface
		{
		public:
			DepthBufferSurface(Graphics3D * graphics, const IntSize & size);
			DepthBufferSurface(const DepthBufferSurface &) = delete;
			virtual ~DepthBufferSurface() {}

			virtual void apply() const override;
		};

		class UISurface : public D3DSurface
		{
			friend class Graphics3D;

		public:
			UISurface(Graphics3D * graphics, UISpace * space);
			UISurface(const UISurface &) = delete;
			virtual ~UISurface();

			virtual void apply() const override;
			virtual void present() const override;
			virtual void requestData(ImageData * data) const override;

		protected:
			void createSwapChain(bool fullscreen);
			void createRenderTarget();
			void releaseRenderTarget();

			static void findPreferredMode(const ComHandle<IDXGIOutput> & output, DXGI_MODE_DESC & mode);

			void onUIResize(Handle<UIResizeMessage> & msg, UISpace & space);
			void onUIFullscreen(Handle<UIFullscreenMessage> & msg, UISpace & space);
			void onUIDestroy(Handle<UIDestroyMessage> & msg, UISpace & space);

			ComHandle<IDXGISwapChain1> swapChain;
			ComHandle<ID3D11RenderTargetView> renderTargetView;

			DXGI_PRESENT_PARAMETERS presentParams;
			UISpace * space;
		};

		class TextureSurface : public D3DSurface
		{
		public:
			TextureSurface(Graphics3D * graphics, const IntSize & size);
			virtual ~TextureSurface() {}

			virtual void apply() const override;
		};

		//---------------------------------------------------------------------------

		class VPShaderProgram : public ShaderProgram
		{
		public:
			VPShaderProgram(const Handle<FxTechnique> & technique, const string & filename, ShaderCodeState state = ShaderCodeState::Compiled) : vs(technique, filename, state), ps(technique, filename, state) {}
			VPShaderProgram(const Handle<FxTechnique> & technique, const Handle<ShaderCodeSet> & codeSet) : vs(technique, codeSet->code.at(ShaderType::Vertex)), ps(technique, codeSet->code.at(ShaderType::Pixel)) {}
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
	}

	inline void print(String & s, const DirectX::XMMATRIX & m)
	{
		s << String::assemble(
			"(",
			DirectX::XMVectorGetX(m.r[0]), ", ",
			DirectX::XMVectorGetY(m.r[0]), ", ",
			DirectX::XMVectorGetZ(m.r[0]), ", ",
			DirectX::XMVectorGetW(m.r[0]), " | ",

			DirectX::XMVectorGetX(m.r[1]), ", ",
			DirectX::XMVectorGetY(m.r[1]), ", ",
			DirectX::XMVectorGetZ(m.r[1]), ", ",
			DirectX::XMVectorGetW(m.r[1]), " | ",

			DirectX::XMVectorGetX(m.r[2]), ", ",
			DirectX::XMVectorGetY(m.r[2]), ", ",
			DirectX::XMVectorGetZ(m.r[2]), ", ",
			DirectX::XMVectorGetW(m.r[2]), " | ",

			DirectX::XMVectorGetX(m.r[3]), ", ",
			DirectX::XMVectorGetY(m.r[3]), ", ",
			DirectX::XMVectorGetZ(m.r[3]), ", ",
			DirectX::XMVectorGetW(m.r[3]), ")"
			);
	}


#ifndef Direct3D
#define Direct3D Direct3D11
	using D3DGraphics = Direct3D11::Graphics3D;
#endif
}

#endif
//---------------------------------------------------------------------------
#endif
