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

			Handle<Debug> debug = nullptr;

		public:
			using Graphics3D::init;
			using Graphics3D::bind;
			using Graphics3D::draw;

			virtual api(direct3d11) void clip(const IntRect & rect) override;
			virtual api(direct3d11) void present() const override;

			virtual api(direct3d11) void printInfo();
			virtual api(direct3d11) void printDebug();
			virtual api(direct3d11) void checkForErrors();

			virtual api(direct3d11) Handle<Image> createImage(const ImageData & data) override;
			virtual api(direct3d11) Handle<Surface> createSurface(UISpace * space) override;
			virtual api(direct3d11) Handle<Surface> createSurface(const IntSize & size, Handle<Image> & image) override;

			virtual Handle<GraphicsDebug> getDebug() const override
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
			friend_graphics_provider(D3DGraphics);

			api(direct3d11) D3DGraphics();
			virtual api(direct3d11) ~D3DGraphics();

			virtual api(direct3d11) Handle<VertexLayout> createVertexLayout(const string & fingerprint) override;
			virtual api(direct3d11) Handle<VertexBuffer> createVertexBuffer(VertexLayout * layout, const VertexData & data, VertexTopology topology) override;
			virtual api(direct3d11) Handle<IndexBuffer> createIndexBuffer(const VertexIndices & indices) override;

			virtual api(direct3d11) UniqueHandle<UniformAdapter> createUniformAdapter(ShaderType shader, int index, size_t size) override;

			virtual api(direct3d11) void initFacilities() override;

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

			api(direct3d11) void initDevice();
			api(direct3d11) void initShaders();

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

		public:
			virtual ~D3DVertexBuffer() {}

			virtual api(direct3d11) void apply() const override;
			virtual api(direct3d11) void draw(const Mesh * mesh) const override;

		protected:
			api(direct3d11) D3DVertexBuffer(D3DGraphics * graphics, VertexLayout * layout, const VertexData & vd, VertexTopology topology);

			D3DGraphics * graphics;
			D3D_PRIMITIVE_TOPOLOGY topology;
			ComHandle<ID3D11Buffer> handle;
		};

		class D3DIndexBuffer : public IndexBuffer
		{
			friend_owned_handle(D3DIndexBuffer, D3DGraphics);

		public:
			virtual ~D3DIndexBuffer() {}

			virtual api(direct3d11) void apply() const override;
			virtual api(direct3d11) void draw(const IndexedMesh * mesh) const override;

		protected:
			api(direct3d11) D3DIndexBuffer(D3DGraphics * graphics, const VertexIndices & indices);

			D3DGraphics * graphics;
			ComHandle<ID3D11Buffer> handle;
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
			D3DShaderProgram(D3DGraphics * graphics, VertexLayout * layout) : graphics(graphics), layout(layout) {}

			D3DGraphics * graphics;
			VertexLayout * layout;
		};

		template<ShaderType type>
		class D3DShader {};

		enum class ShaderCodeState
		{
			Raw,
			Compiled
		};

		template<>
		class D3DShader<ShaderType::Common>
		{
		public:
			api(direct3d11) D3DShader(D3DShaderProgram * program);

		protected:
			api(direct3d11) void read(const String & filename, Handle<ShaderCode> & out);
			api(direct3d11) void compile(const String & filename, const String & entrance, const String & shaderModel, Handle<ShaderCode> & out);

			D3DShaderProgram * program;
		};

		template<>
		class D3DShader<ShaderType::Vertex> : public Shader<ShaderType::Vertex>, public D3DShader<ShaderType::Common>
		{
			api(direct3d11) void init(const Handle<ShaderCode> & code);

		public:
			api(direct3d11) D3DShader(D3DShaderProgram * program, const String & path, ShaderCodeState state = ShaderCodeState::Compiled);
			api(direct3d11) D3DShader(D3DShaderProgram * program, const Handle<ShaderCode> & code);
			virtual ~D3DShader() {}

			virtual api(direct3d11) void apply() const override;

			ComHandle<ID3D11VertexShader> id;
		};

		template<>
		class D3DShader<ShaderType::Pixel> : public Shader<ShaderType::Pixel>, public D3DShader<ShaderType::Common>
		{
			api(direct3d11) void init(const Handle<ShaderCode> & code);

		public:
			api(direct3d11) D3DShader(D3DShaderProgram * program, const String & path, ShaderCodeState state = ShaderCodeState::Compiled);
			api(direct3d11) D3DShader(D3DShaderProgram * program, const Handle<ShaderCode> & code);
			virtual ~D3DShader() {}

			virtual api(direct3d11) void apply() const override;

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

			api(direct3d11) void onUIResize(Handle<UIResizeMessage> & msg, UISpace & space);
			api(direct3d11) void onUIFullscreen(Handle<UIFullscreenMessage> & msg, UISpace & space);
			api(direct3d11) void onUIDestroy(Handle<UIDestroyMessage> & msg, UISpace & space);

			ComHandle<IDXGISwapChain1> _swapChain;

			DXGI_PRESENT_PARAMETERS _presentParams;
			UISpace * _space;
		};

		class TextureSurface : virtual public RenderTargetSurface
		{
			friend class D3DGraphics;

		public:
			api(direct3d11) TextureSurface(D3DGraphics * graphics, const IntSize & size, Handle<Image> & image);
			virtual ~TextureSurface() {}

			virtual api(direct3d11) void clear() const override;
			virtual api(direct3d11) void requestData(ImageData * data) const override;

		protected:
			Handle<D3DImage> _texture;
		};

		//---------------------------------------------------------------------------

		class VPShaderProgram : public D3DShaderProgram
		{
		public:
			VPShaderProgram(D3DGraphics * graphics, VertexLayout * layout, const string & filename, ShaderCodeState state = ShaderCodeState::Compiled) : D3DShaderProgram(graphics, layout),
				vs(this, filename, state),
				ps(this, filename, state)
			{}

			VPShaderProgram(D3DGraphics * graphics, VertexLayout * layout, const Handle<ShaderCodeSet> & codeSet) : D3DShaderProgram(graphics, layout),
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
