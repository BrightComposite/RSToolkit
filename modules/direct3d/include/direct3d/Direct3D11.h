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
		class D3DGraphics;
		class D3DGraphics;
		class D3DVertexLayout;
		class D3DMesh;
		class D3DIndexedMesh;
	}

	link_class(Direct3D11::D3DGraphics, Class<Graphics3D>);

	namespace Direct3D11
	{
		class D3DGraphics : public Graphics3D
		{
			friend_graphics_provider(D3DGraphics);

		public:
			using Graphics3D::bind;
			using Graphics3D::draw;

			virtual void clip(const IntRect & rect) override;
			virtual void present() const override;

			virtual void printInfo();
			virtual void printDebug();
			virtual void checkForErrors();

			virtual Handle<Image> createImage(const ImageData & data) override;

			ComHandle<ID3D11Device2, D3DGraphics> device;
			ComHandle<ID3D11DeviceContext2, D3DGraphics> context;
			ComHandle<ID3D11RasterizerState, D3DGraphics> solidRS;
			ComHandle<ID3D11RasterizerState, D3DGraphics> wiredRS;
			ComHandle<ID3D11DepthStencilState, D3DGraphics> depthState2D;
			ComHandle<ID3D11DepthStencilState, D3DGraphics> depthState3D;
			ComHandle<ID3D11BlendState, D3DGraphics> blendState;
			ComHandle<ID3D11SamplerState, D3DGraphics> linearSampler;
			ComHandle<IDXGIFactory2, D3DGraphics> dxgiFactory;

		protected:
			friend_graphics_provider(D3DGraphics);
			friend class TypedMap<Uniforms::Base, Uniform>;

			D3DGraphics();
			virtual ~D3DGraphics();

			virtual Handle<Surface> createSurface(UISpace * space) override;

			virtual Handle<VertexLayout> createVertexLayout(const string & fingerprint) override;
			virtual Handle<VertexBuffer> createVertexBuffer(VertexLayout * vil, const VertexData & data) override;
			virtual Handle<IndexBuffer> createIndexBuffer(const VertexIndices & indices) override;

			virtual Handle<UniformAdapter> createUniformAdapter(ShaderType shader, int index, size_t size) override;

			virtual void updateBrushState() override;
			virtual void initFacilities() override;

			template<class Program, class ... A, useif <
				is_shader_program<Program>::value,
				can_construct<Handle<ShaderCode>, A>::value...,
				can_construct<Program, D3DGraphics *, VertexLayout *, ShaderCodeSet *>::value
				> endif
			>
			void setShaderProgram(const string & id, VertexLayout * layout, A &&... args)
			{
				shaderPrograms[id] = handle<Program>(this, layout, createCodeSet({handle<ShaderCode>(forward<A>(args))...}));
			}

			void initDevice();
			void initShaders();

			D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
			D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

			ComHandle<ID3D11Debug> debug;
		};

//---------------------------------------------------------------------------

		class D3DImage : public Image
		{
			friend_owned_handle(D3DImage, D3DGraphics);

		public:
			virtual ~D3DImage() {}

			virtual void apply() const override;
			virtual void requestData(ImageData * output) const override;

		protected:
			D3DImage(D3DGraphics * graphics, const ImageData & data);

			ComHandle<ID3D11SamplerState> _state;
			ComHandle<ID3D11ShaderResourceView> _handle;
			D3DGraphics * _ctx;
		};

//---------------------------------------------------------------------------

		class D3DVertexLayout : public VertexLayout
		{
			friend_owned_handle(D3DVertexLayout, D3DGraphics);

		public:
			virtual ~D3DVertexLayout() {}

			virtual void apply();
			virtual void accept(const ShaderCode * code);

		protected:
			D3DVertexLayout(D3DGraphics * graphics, const string & fingerprint);

			ComHandle<ID3D11InputLayout> handle;
			D3DGraphics * graphics;
		};

//---------------------------------------------------------------------------

		class D3DVertexBuffer : public VertexBuffer
		{
			friend_owned_handle(D3DVertexBuffer, D3DGraphics);

		public:
			virtual ~D3DVertexBuffer() {}

			virtual void apply() const override;
			virtual void draw(const Mesh * mesh) const override;

		protected:
			D3DVertexBuffer(D3DGraphics * graphics, VertexLayout * vil, const VertexData & vd, D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			D3DGraphics * graphics;
			D3D_PRIMITIVE_TOPOLOGY topology;
			ComHandle<ID3D11Buffer> handle;
		};

		class D3DIndexBuffer : public IndexBuffer
		{
			friend_owned_handle(D3DIndexBuffer, D3DGraphics);

		public:
			virtual ~D3DIndexBuffer() {}

			virtual void apply() const override;
			virtual void draw(const IndexedMesh * mesh) const override;

		protected:
			D3DIndexBuffer(D3DGraphics * graphics, const VertexIndices & indices);

			D3DGraphics * graphics;
			ComHandle<ID3D11Buffer> handle;
		};

//---------------------------------------------------------------------------
		
		class D3DUniformAdapter : public UniformAdapter
		{
			friend_owned_handle(D3DUniformAdapter, D3DGraphics);

		public:
			virtual ~D3DUniformAdapter() {}

		protected:
			D3DUniformAdapter(D3DGraphics * graphics, ShaderType shader, int index, size_t size);

			virtual void update(const void * data) override;

			ComHandle<ID3D11Buffer> buffer;
			int index;
			D3DGraphics * graphics;
		};

//---------------------------------------------------------------------------
		class D3DShaderProgram : public ShaderProgram
		{
		public:
			D3DShaderProgram(D3DGraphics * graphics, VertexLayout * vil) : graphics(graphics), vil(vil) {}

			D3DGraphics * graphics;
			VertexLayout * vil;
		};

		template<ShaderType type>
		class D3DShader {};

		enum class ShaderCodeState
		{
			Raw,
			Compiled,
			Embedded
		};

		template<>
		class D3DShader<ShaderType::Common>
		{
		public:
			D3DShader(D3DShaderProgram * program);

		protected:
			void read(const String & filename, Handle<ShaderCode> & out);
			void compile(const String & filename, const String & entrance, const String & shaderModel, Handle<ShaderCode> & out);
		
			D3DShaderProgram * program;
		};

		template<>
		class D3DShader<ShaderType::Vertex> : public Shader<ShaderType::Vertex>, public D3DShader<ShaderType::Common>
		{
			void init(const Handle<ShaderCode> & code);

		public:
			D3DShader(D3DShaderProgram * program, const String & path, ShaderCodeState state = ShaderCodeState::Compiled);
			D3DShader(D3DShaderProgram * program, const Handle<ShaderCode> & code);
			virtual ~D3DShader() {}

			virtual void apply() const override;

			ComHandle<ID3D11VertexShader> id;
		};

		template<>
		class D3DShader<ShaderType::Pixel> : public Shader<ShaderType::Pixel>, public D3DShader<ShaderType::Common>
		{
			void init(const Handle<ShaderCode> & code);

		public:
			D3DShader(D3DShaderProgram * program, const String & path, ShaderCodeState state = ShaderCodeState::Compiled);
			D3DShader(D3DShaderProgram * program, const Handle<ShaderCode> & code);
			virtual ~D3DShader() {}

			virtual void apply() const override;

			ComHandle<ID3D11PixelShader> id;
		};

		//---------------------------------------------------------------------------

		class D3DSurface : public Surface
		{
			friend class D3DGraphics;

		public:
			D3DSurface(D3DGraphics * graphics, const IntSize & size);
			virtual ~D3DSurface();

			virtual void clear() const;

		protected:
			D3DGraphics * graphics;
			void createDepthStencil();

			ComHandle<ID3D11DepthStencilView> depthStencilView;
		};

		class DepthBufferSurface : public D3DSurface
		{
		public:
			DepthBufferSurface(D3DGraphics * graphics, const IntSize & size);
			DepthBufferSurface(const DepthBufferSurface &) = delete;
			virtual ~DepthBufferSurface() {}

			virtual void apply() const override;
		};

		class UISurface : public D3DSurface
		{
			friend class D3DGraphics;

		public:
			UISurface(D3DGraphics * graphics, UISpace * space);
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
			TextureSurface(D3DGraphics * graphics, const IntSize & size);
			virtual ~TextureSurface() {}

			virtual void apply() const override;
		};

		//---------------------------------------------------------------------------

		class VPShaderProgram : public D3DShaderProgram
		{
		public:
			VPShaderProgram(D3DGraphics * graphics, VertexLayout * vil, const string & filename, ShaderCodeState state = ShaderCodeState::Compiled) : D3DShaderProgram(graphics, vil),
				vs(this, filename, state),
				ps(this, filename, state)
			{}
			
			VPShaderProgram(D3DGraphics * graphics, VertexLayout * vil, const Handle<ShaderCodeSet> & codeSet) : D3DShaderProgram(graphics, vil),
				vs(this, codeSet->code.at(ShaderType::Vertex)),
				ps(this, codeSet->code.at(ShaderType::Pixel))
			{}

			virtual ~VPShaderProgram() {}

			virtual void apply() const override
			{
				graphics->bind(vs);
				graphics->bind(ps);
			}

		protected:
			Handle<D3DShader<ShaderType::Vertex>> vs;
			Handle<D3DShader<ShaderType::Pixel>> ps;
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
	using namespace Direct3D;
	using GraphicsProvider = CommonGraphicsProvider<D3DGraphics>;
#endif
}

#endif
//---------------------------------------------------------------------------
#endif
