//---------------------------------------------------------------------------

#pragma once

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

#include <message/Subject.h>

#include <windows/ComHandle.h>
#include <windows/ComException.h>

#include <ui/WidgetMessages.h>

//---------------------------------------------------------------------------

#ifdef _DEBUG
#define DEBUG_DX
#endif

//---------------------------------------------------------------------------

namespace asd
{
	using std::thread;

	namespace Direct3D11
	{
		class D3DGraphics;
		class D3DImage;
		class D3DVertexLayout;

		class D3DShader;
		class D3DVertexBuffer;
		class D3DIndexBuffer;
	}

	link_class(direct3d11, Direct3D11::D3DGraphics, Class<Graphics3D>);

	namespace Direct3D11
	{
		class Debug : public GraphicsDebug
		{
		public:
			api(direct3d11) Debug(const ComHandle<ID3D11Device2, D3DGraphics> & device);
			virtual api(direct3d11) ~Debug();

			ComHandle<ID3D11Debug> handle;
		};

		class D3DGraphics : public Graphics3D
		{
			friend_graphics_provider(D3DGraphics);

			handle<Debug> debug = nullptr;

		public:
			using Graphics3D::init;
			using Graphics3D::bind;
			using Graphics3D::draw;

			api(direct3d11) void bind(const D3DShader * shader, ShaderType type);
			api(direct3d11) void bind(const VertexBuffer * buffer);
			api(direct3d11) void bind(const IndexBuffer * buffer);

			virtual api(direct3d11) void clip(const IntRect & rect) override;
			virtual api(direct3d11) void present() const override;

			virtual api(direct3d11) void printInfo();
			virtual api(direct3d11) void printDebug();
			virtual api(direct3d11) void checkForErrors();

			virtual api(direct3d11) handle<Image> createImage(const ImageData & data) override;
			virtual api(direct3d11) handle<Surface> createSurface(UISpace * space) override;
			virtual api(direct3d11) handle<Surface> createSurface(const IntSize & size, handle<Image> & image) override;

			virtual api(direct3d11) handle<Mesh> createMesh(const handle<VertexBuffer> & buffer, VertexTopology topology, uint verticesLocation) override;
			virtual api(direct3d11) handle<IndexedMesh> createMesh(const handle<VertexBuffer> & buffer, const VertexIndices & indices, VertexTopology topology, uint verticesLocation, uint indicesLocation) override;

			virtual handle<GraphicsDebug> getDebug() const override
			{
				return debug;
			}

			ComHandle<ID3D11Device2, D3DGraphics> device;
			ComHandle<ID3D11DeviceContext2, D3DGraphics> context;
			ComHandle<ID3D11RasterizerState, D3DGraphics> solidRS;
			ComHandle<ID3D11RasterizerState, D3DGraphics> transparentRS;
			ComHandle<ID3D11RasterizerState, D3DGraphics> wiredRS;
			ComHandle<ID3D11DepthStencilState, D3DGraphics> depthDisabled;
			ComHandle<ID3D11DepthStencilState, D3DGraphics> depthEnabled;
			ComHandle<ID3D11BlendState, D3DGraphics> blendStateDisabled;
			ComHandle<ID3D11BlendState, D3DGraphics> blendStateEnabled;
			ComHandle<ID3D11SamplerState, D3DGraphics> linearSampler;
			ComHandle<IDXGIFactory2, D3DGraphics> dxgiFactory;

		protected:
			api(direct3d11) D3DGraphics();
			virtual api(direct3d11) ~D3DGraphics();

			virtual api(direct3d11) handle<VertexLayout> createVertexLayout(const string & fingerprint) override;
			virtual api(direct3d11) handle<VertexBuffer> createVertexBuffer(VertexLayout * layout, const VertexData & data) override;
			virtual api(direct3d11) handle<IndexBuffer> createIndexBuffer(const VertexIndices & indices) override;

			virtual api(direct3d11) UniqueHandle<UniformAdapter> createUniformAdapter(const char * name, ShaderType shader, int index, size_t size) override;

			virtual api(direct3d11) void initFacilities() override;

			api(direct3d11) void initDevice();
			api(direct3d11) void initShaders();
			api(direct3d11) void setShaderProgram(const string & id, const handle<ShaderCodeSet> & codeSet);

			map<ShaderType, const D3DShader *> _shaders;

			const VertexBuffer * _vbuffer;
			const IndexBuffer * _ibuffer;

			D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
			D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
		};

//---------------------------------------------------------------------------

		class D3DImage : public Image
		{
			friend class TextureSurface;

		public:
			api(direct3d11) D3DImage(D3DGraphics * graphics, uint width, uint height);
			api(direct3d11) D3DImage(D3DGraphics * graphics, const ImageData & data);

			virtual ~D3DImage() {}

			virtual api(direct3d11) void apply() const override;
			virtual api(direct3d11) void requestData(ImageData * output) const override;

		protected:
			ComHandle<ID3D11Texture2D> _handle;
			ComHandle<ID3D11ShaderResourceView> _resource;
			ComHandle<ID3D11SamplerState> _state;
			D3DGraphics * _ctx;
		};

//---------------------------------------------------------------------------

		class D3DVertexLayout : public VertexLayout
		{
			friend_owned_handle(D3DVertexLayout, D3DGraphics);

		public:
			virtual ~D3DVertexLayout() {}

			virtual api(direct3d11) void apply();
			virtual api(direct3d11) void accept(const ShaderCode * code);

		protected:
			api(direct3d11) D3DVertexLayout(D3DGraphics * graphics, const string & fingerprint);

			ComHandle<ID3D11InputLayout> handle;
			D3DGraphics * graphics;
		};

//---------------------------------------------------------------------------

		class D3DVertexBuffer : public VertexBuffer
		{
			friend_owned_handle(D3DVertexBuffer, D3DGraphics);
			friend class D3DGraphics;

		public:
			virtual ~D3DVertexBuffer() {}

			virtual api(direct3d11) void apply() const override;

		protected:
			api(direct3d11) D3DVertexBuffer(D3DGraphics * graphics, VertexLayout * layout, const VertexData & vd);

			D3DGraphics * graphics;
			ComHandle<ID3D11Buffer> handle;
		};

		class D3DIndexBuffer : public IndexBuffer
		{
			friend_owned_handle(D3DIndexBuffer, D3DGraphics);
			friend class D3DGraphics;

		public:
			virtual ~D3DIndexBuffer() {}

			virtual api(direct3d11) void apply() const override;

		protected:
			api(direct3d11) D3DIndexBuffer(D3DGraphics * graphics, const VertexIndices & indices);

			D3DGraphics * graphics;
			ComHandle<ID3D11Buffer> handle;
		};

		class D3DMeshTrait
		{
		public:
			api(direct3d11) D3DMeshTrait(D3DGraphics * graphics, VertexTopology topology);

		protected:
			D3D_PRIMITIVE_TOPOLOGY topology;
			D3DGraphics * graphics;
		};

		class D3DMesh : public Mesh, public D3DMeshTrait
		{
		public:
			D3DMesh(D3DGraphics * graphics, const handle<VertexBuffer> & vbuffer, VertexTopology topology, uint verticesLocation) : Mesh(vbuffer, topology, verticesLocation), D3DMeshTrait(graphics, topology) {}

			virtual api(direct3d11) void draw() const override;
		};

		class D3DIndexedMesh : public IndexedMesh, public D3DMeshTrait
		{
		public:
			D3DIndexedMesh(D3DGraphics * graphics, const handle<VertexBuffer> & vbuffer, const handle<IndexBuffer> & ibuffer, VertexTopology topology, uint verticesLocation, uint indicesLocation) : IndexedMesh(vbuffer, ibuffer, topology, verticesLocation, indicesLocation), D3DMeshTrait(graphics, topology) {}

			virtual api(direct3d11) void draw() const override;
		};

//---------------------------------------------------------------------------

		class D3DUniformAdapter : public UniformAdapter
		{
			friend_owned_handle(D3DUniformAdapter, D3DGraphics);

		public:
			virtual ~D3DUniformAdapter() {}

			virtual api(direct3d11) void update(const void * data) override;

		protected:
			api(direct3d11) D3DUniformAdapter(D3DGraphics * graphics, ShaderType shader, int index, size_t size);

			ComHandle<ID3D11Buffer> buffer;
			int index;
			D3DGraphics * graphics;
		};

//---------------------------------------------------------------------------

		class D3DShaderProgram : public ShaderProgram
		{
		public:
			api(direct3d11) D3DShaderProgram(D3DGraphics * graphics, const handle<ShaderCodeSet> & codeSet);
			api(direct3d11) ~D3DShaderProgram();

			virtual api(direct3d11) void apply() const override;

			D3DGraphics * graphics;
			D3DVertexLayout * layout;

		protected:
			Map<ShaderType, D3DShader> _shaders;
		};

		class D3DShader : public Shader
		{
		public:
			api(direct3d11) D3DShader(D3DShaderProgram * program);

			virtual void apply() const = 0;

		protected:
			api(direct3d11) void read(const String & filename, handle<ShaderCode> & out);
			api(direct3d11) void compile(const String & filename, const String & entrance, const String & shaderModel, handle<ShaderCode> & out);

			D3DShaderProgram * program;
		};

		class D3DVertexShader : public D3DShader
		{
			friend class D3DGraphics;

			api(direct3d11) void init(const handle<ShaderCode> & code);

		public:
			api(direct3d11) D3DVertexShader(D3DShaderProgram * program, const String & path, ShaderCodeState state = ShaderCodeState::Compiled);
			api(direct3d11) D3DVertexShader(D3DShaderProgram * program, const handle<ShaderCode> & code);
			virtual ~D3DVertexShader() {}

			virtual api(direct3d11) void apply() const;

		protected:
			ComHandle<ID3D11VertexShader> id;
		};

		class D3DPixelShader : public D3DShader
		{
			friend class D3DGraphics;

			api(direct3d11) void init(const handle<ShaderCode> & code);

		public:
			api(direct3d11) D3DPixelShader(D3DShaderProgram * program, const String & path, ShaderCodeState state = ShaderCodeState::Compiled);
			api(direct3d11) D3DPixelShader(D3DShaderProgram * program, const handle<ShaderCode> & code);
			virtual ~D3DPixelShader() {}

			virtual api(direct3d11) void apply() const;

		protected:
			ComHandle<ID3D11PixelShader> id;
		};

		//---------------------------------------------------------------------------

		class D3DSurface : public Surface
		{
			deny_copy(D3DSurface);

		public:
			api(direct3d11) D3DSurface(D3DGraphics * graphics, const IntSize & size);
			virtual api(direct3d11) ~D3DSurface();

			virtual api(direct3d11) void clear() const override;
			virtual api(direct3d11) void present() const override;

		protected:
			void createDepthStencil();

			D3DGraphics * _graphics;
			ComHandle<ID3D11DepthStencilView> _depthStencilView;
		};

		class RenderTargetSurface : public D3DSurface
		{
			deny_copy(RenderTargetSurface);

		public:
			api(direct3d11) RenderTargetSurface(D3DGraphics * graphics, const IntSize & size);
			virtual api(direct3d11) ~RenderTargetSurface();

			virtual api(direct3d11) void apply() const override;

		protected:
			ComHandle<ID3D11RenderTargetView> _renderView;
		};

		class DepthSurface : public D3DSurface
		{
			deny_copy(DepthSurface);

		public:
			api(direct3d11) DepthSurface(D3DGraphics * graphics, const IntSize & size);
			virtual ~DepthSurface() {}

			virtual api(direct3d11) void apply() const override;
			virtual api(direct3d11) void clear() const override;
		};

		class UISurface : public RenderTargetSurface, public Connector
		{
			deny_copy(UISurface);

		public:
			api(direct3d11) UISurface(D3DGraphics * graphics, UISpace * space);
			virtual ~UISurface() {}

			virtual api(direct3d11) void present() const override;
			virtual api(direct3d11) void requestData(ImageData * data) const override;

			virtual api(direct3d11) void resize() override;

		protected:
			api(direct3d11) void createSwapChain(bool fullscreen);
			api(direct3d11) void createRenderTarget();
			api(direct3d11) void releaseRenderTarget();

			static api(direct3d11) void findPreferredMode(const ComHandle<IDXGIOutput> & output, DXGI_MODE_DESC & mode);

			api(direct3d11) void onUIResize(handle<UIResizeMessage> & msg, UISpace & space);
			api(direct3d11) void onUIFullscreen(handle<UIFullscreenMessage> & msg, UISpace & space);
			api(direct3d11) void onUIDestroy(handle<UIDestroyMessage> & msg, UISpace & space);

			ComHandle<IDXGISwapChain1> _swapChain;

			DXGI_PRESENT_PARAMETERS _presentParams;
			UISpace * _space;
		};

		class TextureSurface : public RenderTargetSurface
		{
			friend class D3DGraphics;

		public:
			api(direct3d11) TextureSurface(D3DGraphics * graphics, const IntSize & size, handle<Image> & image);
			virtual ~TextureSurface() {}

			virtual api(direct3d11) void clear() const override;
			virtual api(direct3d11) void requestData(ImageData * data) const override;

		protected:
			handle<D3DImage> _texture;
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
