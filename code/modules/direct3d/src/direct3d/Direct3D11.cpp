//---------------------------------------------------------------------------

#include <direct3d/Direct3D11.h>
#include <dxgidebug.h>

#include <windows/RectAdapter.h>

//---------------------------------------------------------------------------

namespace asd
{
	implement_link(Direct3D::D3DGraphics);

	namespace Direct3D
	{
		using namespace DirectX;

		class FillModeState : public State<FillMode>
		{
		public:
			FillModeState(D3DGraphics * graphics) : State<FillMode>(FillMode::Solid), graphics(graphics) {}

		protected:
			virtual void change() override
			{
				if(_state == FillMode::Solid)
					graphics->context->RSSetState(graphics->solidRS);
				else
					graphics->context->RSSetState(graphics->wiredRS);
			}

			D3DGraphics * graphics;
		};

		class DepthTestState : public State<bool>
		{
		public:
			DepthTestState(D3DGraphics * graphics) : State<bool>(true), graphics(graphics) {}

		protected:
			virtual void change() override
			{
				if(_state)
					graphics->context->OMSetDepthStencilState(graphics->depthEnabled, 0);
				else
					graphics->context->OMSetDepthStencilState(graphics->depthDisabled, 0);

				graphics->setDepth(1.0f);
			}

			D3DGraphics * graphics;
		};

		class BlendState : public State<bool>
		{
		public:
			BlendState(D3DGraphics * graphics) : State<bool>(true), graphics(graphics) {}

		protected:
			virtual void change() override
			{
				if(_state)
					graphics->context->OMSetBlendState(graphics->blendStateEnabled, nullptr, 0xFFFFFFFF);
				else
					graphics->context->OMSetBlendState(graphics->blendStateDisabled, nullptr, 0xFFFFFFFF);

				graphics->setDepth(1.0f);
			}

			D3DGraphics * graphics;
		};

		D3DGraphics::D3DGraphics()
		{
			setclass(D3DGraphics);

			initDevice();

			_fillMode = handle<FillModeState>(this);
			_depthTestMode = handle<DepthTestState>(this);
			_blendMode = handle<BlendState>(this);
		}

		Debug::Debug(const ComHandle<ID3D11Device2, D3DGraphics> & device)
		{
			device.queryInterface(handle);
		}

		Debug::~Debug()
		{
		#ifdef DEBUG_DX
			handle->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		#endif
		}

		D3DGraphics::~D3DGraphics()
		{
			if(context != nullptr)
			{
				context->OMSetRenderTargets(0, 0, 0);
				context->Flush();
				context->ClearState();
			}
		}

		void D3DGraphics::initDevice()
		{
			UINT createDeviceFlags = 0;

		#ifdef DEBUG_DX
			createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		#endif

			const D3D_FEATURE_LEVEL featureLevels[] =
			{
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
			};

			const UINT numFeatureLevels = 3;

			const D3D_DRIVER_TYPE driverTypes[] =
			{
				D3D_DRIVER_TYPE_HARDWARE,
				D3D_DRIVER_TYPE_WARP,
				D3D_DRIVER_TYPE_REFERENCE,
			};

			const UINT numDriverTypes = 3;

			ComHandle<ID3D11Device> ndevice;
			ComHandle<ID3D11DeviceContext> ncontext;

			HRESULT hr = S_OK;

			for(auto & dt : driverTypes)
			{
				driverType = dt;

				hr = D3D11CreateDevice(nullptr, driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &ndevice, &featureLevel, &ncontext);

				if(SUCCEEDED(hr))
					break;
			}

			com_assert(hr);

			ndevice.queryInterface(device); //TODO Add interface checking
			ncontext.queryInterface(context);

		#ifdef DEBUG_DX
			debug.init(device);
		#endif

			D3D11_RASTERIZER_DESC rd;
			ZeroMemory(&rd, sizeof(rd));

			rd.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		#ifdef DEBUG_DX
			rd.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		#else
			rd.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		#endif
			rd.FrontCounterClockwise = false;
			rd.DepthBias = 0;
			rd.SlopeScaledDepthBias = 0.0f;
			rd.DepthBiasClamp = 0.0f;
			rd.DepthClipEnable = true;
			rd.ScissorEnable = true;
			rd.MultisampleEnable = false;
			rd.AntialiasedLineEnable = false;

			device->CreateRasterizerState(&rd, &solidRS);

			rd.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
			device->CreateRasterizerState(&rd, &transparentRS);

			rd.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
			device->CreateRasterizerState(&rd, &wiredRS);
			context->RSSetState(solidRS);

			D3D11_BLEND_DESC bd;
			ZeroMemory(&bd, sizeof(bd));

			bd.AlphaToCoverageEnable = FALSE;

			for(int i = 0; i < 8; ++i)
			{
				auto & rt = bd.RenderTarget[i];

				rt.BlendEnable = TRUE;
				rt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
				rt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				rt.BlendOp = D3D11_BLEND_OP_ADD;
				rt.SrcBlendAlpha = D3D11_BLEND_ONE;
				rt.DestBlendAlpha = D3D11_BLEND_ONE;
				rt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
				rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			}

			device->CreateBlendState(&bd, &blendStateEnabled);
			/*
			for(int i = 0; i < 8; ++i)
			{
				auto & rt = bd.RenderTarget[i];
				rt.DestBlendAlpha = D3D11_BLEND_ZERO;
			}

			device->CreateBlendState(&bd, &blendStateEnabled);
			*/
			for(int i = 0; i < 8; ++i)
			{
				auto & rt = bd.RenderTarget[i];
				rt.BlendEnable = FALSE;
			}

			device->CreateBlendState(&bd, &blendStateDisabled);

			context->OMSetBlendState(blendStateEnabled, nullptr, 0xFFFFFFFF);

			D3D11_DEPTH_STENCIL_DESC dsd;
			dsd.DepthEnable = FALSE;
			dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsd.DepthFunc = D3D11_COMPARISON_ALWAYS;
			dsd.StencilEnable = FALSE;
			dsd.StencilReadMask = 0xFF;
			dsd.StencilWriteMask = 0xFF;

			dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			device->CreateDepthStencilState(&dsd, &depthDisabled);

			dsd.DepthEnable = TRUE;
			dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

			device->CreateDepthStencilState(&dsd, &depthEnabled);

			context->OMSetDepthStencilState(depthEnabled, 0);

			ComHandle<IDXGIAdapter> dxgiAdapter;
			ComHandle<IDXGIDevice1> d1;

			device.queryInterface(d1);
			d1->GetAdapter(&dxgiAdapter);
			dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), dxgiFactory.pointer());

			ComHandle<IDXGIDevice3> d3;
			device.queryInterface(d3);

			d3->SetMaximumFrameLatency(1);
		}

		void D3DGraphics::initFacilities()
		{
			initShaders();
			Graphics3D::initFacilities();
		}

		void D3DGraphics::bind(const D3DShader * shader, ShaderType type)
		{
			auto & place = _shaders[type];

			if(place != shader)
			{
				place = shader;

				switch(type)
				{
					case ShaderType::Vertex:
						context->VSSetShader(shader != nullptr ? &*static_cast<const D3DVertexShader *>(shader)->id : nullptr, nullptr, 0);
						break;
					case ShaderType::Pixel:
						context->PSSetShader(shader != nullptr ? &*static_cast<const D3DPixelShader *>(shader)->id : nullptr, nullptr, 0);
						break;
				}
			}
		}

		void D3DGraphics::bind(const VertexBuffer * buffer)
		{
			if(_vbuffer != buffer)
			{
				_vbuffer = buffer;
				_vbuffer->apply();
			}
		}

		void D3DGraphics::bind(const IndexBuffer * buffer)
		{
			if(_ibuffer != buffer)
			{
				_ibuffer = buffer;
				_ibuffer->apply();
			}
		}

		void D3DGraphics::printInfo() {}
		void D3DGraphics::printDebug() {}
		void D3DGraphics::checkForErrors() {}

		handle<Image> D3DGraphics::createImage(const ImageData & data)
		{
			return handle<D3DImage>(this, data);
		}

		handle<VertexLayout> D3DGraphics::createVertexLayout(const string & fingerprint)
		{
			return handle<D3DVertexLayout, D3DGraphics>(this, fingerprint);
		}

		handle<VertexBuffer> D3DGraphics::createVertexBuffer(VertexLayout * layout, const VertexData & data)
		{
			return handle<D3DVertexBuffer, D3DGraphics>(this, layout, data);
		}

		handle<IndexBuffer> D3DGraphics::createIndexBuffer(const VertexIndices & indices)
		{
			return handle<D3DIndexBuffer, D3DGraphics>(this, indices);
		}

		handle<Mesh> D3DGraphics::createMesh(const handle<VertexBuffer> & buffer, VertexTopology topology, uint verticesLocation)
		{
			return handle<D3DMesh>(this, buffer, topology, verticesLocation);
		}

		handle<IndexedMesh> D3DGraphics::createMesh(const handle<VertexBuffer> & buffer, const VertexIndices & indices, VertexTopology topology, uint verticesLocation, uint indicesLocation)
		{
			return handle<D3DIndexedMesh>(this, buffer, createIndexBuffer(indices), topology, verticesLocation, indicesLocation);
		}

		UniqueHandle<UniformAdapter> D3DGraphics::createUniformAdapter(const char * name, ShaderType shader, int index, size_t size)
		{
			return UniqueHandle<D3DUniformAdapter, D3DGraphics>(this, shader, index, size);
		}

		handle<Surface> D3DGraphics::createSurface(UISpace * space)
		{
			return handle<UISurface>(this, space);
		}

		handle<Surface> D3DGraphics::createSurface(const IntSize & size, handle<Image> & image)
		{
			return handle<TextureSurface>(this, size, image);
		}

		void D3DGraphics::present() const
		{
			_surface->present();
		}

		void D3DGraphics::clip(const IntRect & rect)
		{
			if(_clipRect == rect)
				return;

			RectAdapter r;

			_clipRect = rect;
			context->RSSetScissorRects(1, &r.set(_clipRect));
		}
	}
}