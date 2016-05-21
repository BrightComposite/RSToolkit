//---------------------------------------------------------------------------

#include <direct3d/Direct3D11.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>

#include <ui/UISpace.h>
#include <windows/RectAdapter.h>

//---------------------------------------------------------------------------

namespace Rapture
{
#ifdef _DEBUG
#define DEBUG_DX
#endif

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
			DepthTestState(D3DGraphics * graphics) : State<bool>(false), graphics(graphics) {}

		protected:
			virtual void change() override
			{
				if(_state)
					graphics->context->OMSetDepthStencilState(graphics->depthState3D, 0);
				else
					graphics->context->OMSetDepthStencilState(graphics->depthState2D, 0);

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
		}

		D3DGraphics::~D3DGraphics()
		{
			if(context != nullptr)
			{
				context->OMSetRenderTargets(0, 0, 0);
				context->Flush();

				context->ClearState();
			}

		#ifdef DEBUG_DX
			debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		#endif
		}

		void D3DGraphics::initDevice()
		{
			HRESULT hr = S_OK;
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

			for(auto & dt : driverTypes)
			{
				driverType = dt;

				hr = D3D11CreateDevice(nullptr, driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &ndevice, &featureLevel, &ncontext);

				if(SUCCEEDED(hr))
					break;
			}

			com_assert(hr);

			device = ndevice;
			context = ncontext;

		#ifdef DEBUG_DX
			debug = device;
		#endif

			D3D11_RASTERIZER_DESC rd;
			ZeroMemory(&rd, sizeof(rd));

			rd.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
			rd.FrontCounterClockwise = false;
			rd.DepthBias = 0;
			rd.SlopeScaledDepthBias = 0.0f;
			rd.DepthBiasClamp = 0.0f;
			rd.DepthClipEnable = true;
			rd.ScissorEnable = true;
			rd.MultisampleEnable = false;
			rd.AntialiasedLineEnable = false;

			device->CreateRasterizerState(&rd, &solidRS);

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
				rt.DestBlendAlpha = D3D11_BLEND_ZERO;
				rt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
				rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			}

			float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};

			device->CreateBlendState(&bd, &blendState);
			context->OMSetBlendState(blendState, blendFactor, 0xFFFFFFFF);

			D3D11_DEPTH_STENCIL_DESC dsd;
			dsd.DepthEnable = TRUE;
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

			device->CreateDepthStencilState(&dsd, &depthState2D);

			dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

			device->CreateDepthStencilState(&dsd, &depthState3D);

			context->OMSetDepthStencilState(depthState2D, 0);

			ComHandle<IDXGIAdapter> dxgiAdapter;
			ComHandle<IDXGIDevice1>(device)->GetAdapter(&dxgiAdapter);
			dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), dxgiFactory.pointer());

			ComHandle<IDXGIDevice3> dxgiDevice;
			dxgiDevice = device;

			dxgiDevice->SetMaximumFrameLatency(1);
		}

		void D3DGraphics::initFacilities()
		{
			initShaders();
			Graphics3D::initFacilities();
			updateBrushState();
		}

		void D3DGraphics::printInfo() {}
		void D3DGraphics::printDebug() {}
		void D3DGraphics::checkForErrors() {}

		Handle<Image> D3DGraphics::createImage(const ImageData & data)
		{
			return Handle<D3DImage, D3DGraphics>(this, data);
		}

		Handle<VertexLayout> D3DGraphics::createVertexLayout(const string & fingerprint)
		{
			return Handle<D3DVertexLayout, D3DGraphics>(this, fingerprint);
		}

		Handle<VertexBuffer> D3DGraphics::createVertexBuffer(VertexLayout * vil, const VertexData & data)
		{
			return Handle<D3DVertexBuffer, D3DGraphics>(this, vil, data);
		}

		Handle<IndexBuffer> D3DGraphics::createIndexBuffer(const VertexIndices & indices)
		{
			return Handle<D3DIndexBuffer, D3DGraphics>(this, indices);
		}

		Handle<UniformAdapter> D3DGraphics::createUniformAdapter(ShaderType shader, int index, size_t size)
		{
			return Handle<D3DUniformAdapter, D3DGraphics>(this, shader, index, size);
		}

		Handle<Surface> D3DGraphics::createSurface(UISpace * space)
		{
			Handle<UISurface, D3DGraphics> s(this, space);

			connect(*space, s, &UISurface::onUIResize);
			connect(*space, s, &UISurface::onUIFullscreen);
			connect(*space, s, &UISurface::onUIDestroy);

			return s;
		}

		void D3DGraphics::updateBrushState()
		{
			updateUniform<Uniforms::BrushOptions>(color(), lineWidth());
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

		D3DVertexLayout::D3DVertexLayout(D3DGraphics * graphics, const string & fingerprint) : VertexLayout(fingerprint), graphics(graphics) {}

		void D3DVertexLayout::apply()
		{
			graphics->context->IASetInputLayout(handle);
		}

		void D3DVertexLayout::accept(const ShaderCode * code)
		{
			if(handle != nullptr)
				return;

			static const DXGI_FORMAT formats[] = {
				DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_R32_FLOAT,
				DXGI_FORMAT_R32G32_FLOAT,
				DXGI_FORMAT_R32G32B32_FLOAT,
				DXGI_FORMAT_R32G32B32A32_FLOAT
			};

			vector<D3D11_INPUT_ELEMENT_DESC> descs;
			uint stride = 0;

			for(auto * vie : elements)
			{
				descs.push_back({vie->semantic, vie->index, formats[vie->units], 0, stride, D3D11_INPUT_PER_VERTEX_DATA, 0});
				stride += vie->units * sizeof(float);
			}

			com_assert(
				graphics->device->CreateInputLayout(descs.data(), static_cast<uint>(descs.size()), code->data, code->size, &handle),
				"Can't create vertex input layout for shader"
			);
		}

		D3DVertexBuffer::D3DVertexBuffer(D3DGraphics * graphics, VertexLayout * vil, const VertexData & vd, D3D_PRIMITIVE_TOPOLOGY topology) : VertexBuffer(vil, vd), graphics(graphics), topology(topology)
		{
			if(vd.size % vil->stride != 0)
				throw Exception("Size of vertex buffer doesn't matches its vertex input layout");

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = static_cast<uint>(vd.size);
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sdata;
			ZeroMemory(&sdata, sizeof(sdata));
			sdata.pSysMem = vd.data;

			com_assert(
				graphics->device->CreateBuffer(&bd, &sdata, &handle),
				"Can't create vertex buffer!"
				);
		}

		void D3DVertexBuffer::apply() const
		{
			uint stride = vil->stride;
			uint offset = 0;

			graphics->context->IASetVertexBuffers(0, 1, &handle, &stride, &offset);
			graphics->context->IASetPrimitiveTopology(topology);
		}

		void D3DVertexBuffer::draw(const Mesh * mesh) const
		{
			graphics->context->Draw(verticesCount - mesh->verticesLocation, mesh->verticesLocation);
		}

		D3DIndexBuffer::D3DIndexBuffer(D3DGraphics * graphics, const VertexIndices & indices) : IndexBuffer(indices), graphics(graphics)
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));

			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = static_cast<uint>(size) * 2;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sdata;
			ZeroMemory(&sdata, sizeof(sdata));
			sdata.pSysMem = indices.data();

			com_assert(
				graphics->device->CreateBuffer(&bd, &sdata, &handle),
				"Can't create index buffer!"
				);
		}

		void D3DIndexBuffer::apply() const
		{
			graphics->context->IASetIndexBuffer(handle, DXGI_FORMAT_R16_UINT, 0);
		}

		void D3DIndexBuffer::draw(const IndexedMesh * mesh) const
		{
			graphics->context->DrawIndexed(size - mesh->indicesLocation, mesh->indicesLocation, mesh->verticesLocation);
		}

		D3DShader<ShaderType::Common>::D3DShader(D3DShaderProgram * program) : program(program) {}

		void D3DShader<ShaderType::Common>::read(const String & filename, Handle<ShaderCode> & out)
		{
			ComHandle<ID3DBlob> code;

			com_assert(
				D3DReadFileToBlob(widen(filename).c_str(), &code),
				"Can't read shader from file: ", filename
				);

			out.init(code->GetBufferPointer(), code->GetBufferSize());
		}

		void D3DShader<ShaderType::Common>::compile(const String & filename, const String & entrance, const String & shaderModel, Handle<ShaderCode> & out)
		{
			ComHandle<ID3DBlob> code;
			DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
		#ifdef DEBUG_DX
			flags |= D3DCOMPILE_DEBUG;
		#endif
			ComHandle<ID3DBlob> error;
			HRESULT hr = D3DCompileFromFile(widen(filename).c_str(), nullptr, nullptr, entrance, shaderModel, flags, 0, &code, &error);

			com_assert(hr, (char *)error->GetBufferPointer());

			out.init(code->GetBufferPointer(), code->GetBufferSize());
		}

		//#define COMPILE_SHADERS

		D3DShader<ShaderType::Vertex>::D3DShader(D3DShaderProgram * program, const String & path, ShaderCodeState state) : D3DShader<ShaderType::Common>(program)
		{
			Handle<ShaderCode> code;

			switch(state)
			{
				case ShaderCodeState::Raw:
					compile(path + "_vs.fx", "main", "vs_4_0", code);
					break;

				case ShaderCodeState::Compiled:
					read(path + "_vs.cso", code);
					break;

				case ShaderCodeState::Embedded:
					code = program->graphics->getShaderCode(path, ShaderType::Vertex);
					break;
			}

			init(code);
		}

		D3DShader<ShaderType::Vertex>::D3DShader(D3DShaderProgram * program, const Handle<ShaderCode> & code) : D3DShader<ShaderType::Common>(program)
		{
			init(code);
		}

		void D3DShader<ShaderType::Vertex>::apply() const
		{
			program->vil->apply();
			program->graphics->context->VSSetShader(id, nullptr, 0);
		}

		void D3DShader<ShaderType::Vertex>::init(const Handle<ShaderCode> & code)
		{
			HRESULT hr = S_OK;

			com_assert(
				program->graphics->device->CreateVertexShader(code->data, code->size, nullptr, &id),
				"Can't create vertex shader!"
				);

			program->vil->accept(code);
		}

		D3DShader<ShaderType::Pixel>::D3DShader(D3DShaderProgram * program, const String & path, ShaderCodeState state) : D3DShader<ShaderType::Common>(program)
		{
			Handle<ShaderCode> code;

			switch(state)
			{
				case ShaderCodeState::Raw:
					compile(path + "_ps.fx", "main", "ps_4_0", code);
					break;

				case ShaderCodeState::Compiled:
					read(path + "_ps.cso", code);
					break;

				case ShaderCodeState::Embedded:
					code = program->graphics->getShaderCode(path, ShaderType::Pixel);
					break;
			}

			init(code);
		}

		D3DShader<ShaderType::Pixel>::D3DShader(D3DShaderProgram * program, const Handle<ShaderCode> & code) : D3DShader<ShaderType::Common>(program)
		{
			init(code);
		}

		void D3DShader<ShaderType::Pixel>::apply() const
		{
			program->graphics->context->PSSetShader(id, nullptr, 0);
		}

		void D3DShader<ShaderType::Pixel>::init(const Handle<ShaderCode> & code)
		{
			com_assert(
				program->graphics->device->CreatePixelShader(code->data, code->size, nullptr, &id),
				"Can't create pixel shader!"
				);
		}

		D3DImage::D3DImage(D3DGraphics * graphics, const ImageData & data) : Image(graphics, data), _ctx(graphics)
		{
			ComHandle<ID3D11Texture2D> tex;

			D3D11_TEXTURE2D_DESC texDesc;
			ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));

			texDesc.Width = _width;
			texDesc.Height = _height;
			texDesc.ArraySize = 1;
			texDesc.MipLevels = 1;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.CPUAccessFlags = 0;
			texDesc.MiscFlags = 0;

			_format = data.format;

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

			D3D11_SUBRESOURCE_DATA texInitData;
			ZeroMemory(&texInitData, sizeof(D3D11_SUBRESOURCE_DATA));

			texInitData.pSysMem = newData->data;
			texInitData.SysMemPitch = _width * preferredBpp(_format);
			texInitData.SysMemSlicePitch = texInitData.SysMemPitch;

			com_assert(
				graphics->device->CreateTexture2D(&texDesc, &texInitData, &tex)
				);

			com_assert(
				graphics->device->CreateShaderResourceView(tex, nullptr, &_handle)
				);

			D3D11_SAMPLER_DESC sampDesc;
			ZeroMemory(&sampDesc, sizeof(sampDesc));
			sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			com_assert(
				graphics->device->CreateSamplerState(&sampDesc, &_state)
				);
		}

		void D3DImage::apply() const
		{
			_ctx->context->PSSetShaderResources(0, 1, &_handle);
			_ctx->context->PSSetSamplers(0, 1, &_state);
		}

		void D3DImage::requestData(ImageData * output) const
		{
			if(output == nullptr)
				throw Exception("Output image buffer should be not-null!");

			ComHandle<ID3D11Texture2D> texture;
			D3D11_TEXTURE2D_DESC texDesc;

			ComHandle<ID3D11Resource> res;
			ComHandle<ID3D11Texture2D> ownTexture;

			_handle->GetResource(&res);
			ownTexture = res;

			ownTexture->GetDesc(&texDesc);

			texDesc.BindFlags = 0;
			texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			texDesc.Usage = D3D11_USAGE_STAGING;

			com_assert(
				_ctx->device->CreateTexture2D(&texDesc, nullptr, &texture)
				);

			_ctx->context->CopyResource(texture, ownTexture);

			D3D11_MAPPED_SUBRESOURCE resource;
			com_assert(
				_ctx->context->Map(texture, 0, D3D11_MAP_READ, 0, &resource)
				);

			output->width = _width;
			output->height = _height;
			output->format = ImageFormat::rgba;

			const uint pitch = output->width * 4;

			output->alloc();

			uint8_t * src = static_cast<uint8_t *>(resource.pData);
			uint8_t * dst = output->data;

			for(uint y = 0; y < output->height; ++y, src += resource.RowPitch, dst += pitch)
				memcpy(dst, src, pitch);

			_ctx->context->Unmap(texture, 0);
		}

	#define USE_3D

		UISurface::UISurface(D3DGraphics * graphics, UISpace * space) : D3DSurface(graphics, space->size()), space(space)
		{
			createSwapChain(false);
			ZeroMemory(&presentParams, sizeof(presentParams));
		}

		UISurface::~UISurface() {}

		void UISurface::createSwapChain(bool fullscreen)
		{
			releaseRenderTarget();

			ComHandle<IDXGISwapChain> sc;

			DXGI_SWAP_CHAIN_DESC sd;
			ZeroMemory(&sd, sizeof(sd));

			auto & md = sd.BufferDesc;

			md.Width = viewport.width();
			md.Height = viewport.height();
			md.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

			if(fullscreen)
			{
				md.RefreshRate.Numerator = 60;
				md.RefreshRate.Denominator = 1;
			}

			if(swapChain != nullptr)
			{
				if(fullscreen)
				{
					ComHandle<IDXGIOutput> output;

					com_assert(
						swapChain->GetContainingOutput(&output)
						);

					output->FindClosestMatchingMode(&md, &md, graphics->device);
					//findPreferredMode(output, md);
				}
				else
					swapChain->SetFullscreenState(FALSE, nullptr);

				swapChain = nullptr;
			}

			sd.BufferCount = 1;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.OutputWindow = space->handle();
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.Windowed = TRUE;
			sd.Flags = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			//sd.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

			com_assert(
				graphics->dxgiFactory->CreateSwapChain(graphics->device, &sd, &sc)
				);

			swapChain = sc;
			createRenderTarget();

			if(fullscreen)
				swapChain->SetFullscreenState(TRUE, nullptr);
		}

		void UISurface::createRenderTarget()
		{
			createDepthStencil();

			D3D11_VIEWPORT vp;
			vp.Width = static_cast<float>(viewport.width());
			vp.Height = static_cast<float>(viewport.height());
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;

			graphics->context->RSSetViewports(1, &vp);

			{
				ComHandle<ID3D11Texture2D> buffer;

				com_assert(
					swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), buffer.pointer())
					);

				com_assert(
					graphics->device->CreateRenderTargetView(buffer, nullptr, &renderTargetView)
					);
			}

		#ifdef USE_3D
			graphics->context->OMSetRenderTargets(1, &renderTargetView, nullptr);
		#else
			graphics->context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
		#endif
		}

		void UISurface::releaseRenderTarget()
		{
			if(renderTargetView != nullptr || depthStencilView != nullptr)
			{
				graphics->context->OMSetRenderTargets(0, 0, 0);
				graphics->context->Flush();
				renderTargetView = nullptr;
				depthStencilView = nullptr;
			}
		}

		void UISurface::findPreferredMode(const ComHandle<IDXGIOutput> & output, DXGI_MODE_DESC & mode)
		{
			uint num;
			output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &num, nullptr);

			auto descs = Memory<DXGI_MODE_DESC>::allocate(num);

			output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &num, descs);

			for(uint i = 0; i < num; ++i)
			{
				auto & desc = descs[i];

				if(desc.RefreshRate.Numerator / desc.RefreshRate.Denominator > 50)
				{
					if(desc.Width >= mode.Width || desc.Height >= mode.Height)
					{
						mode = desc;
						return;
					}
				}
			}

			Memory<DXGI_MODE_DESC>::free(descs);
		}

		void UISurface::present() const
		{
			swapChain->Present(0, 0);
			//graphics->context->DiscardView(renderTargetView);
			apply();
		}

		void UISurface::requestData(ImageData * output) const
		{
			if(output == nullptr)
				throw Exception("Output image buffer should be not-null!");

			ComHandle<ID3D11Texture2D> buffer;

			com_assert(
				swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), buffer.pointer())
				);

			ComHandle<ID3D11Texture2D> texture;
			D3D11_TEXTURE2D_DESC texDesc;
			buffer->GetDesc(&texDesc);

			texDesc.BindFlags = 0;
			texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			texDesc.Usage = D3D11_USAGE_STAGING;

			com_assert(
				graphics->device->CreateTexture2D(&texDesc, nullptr, &texture)
				);

			graphics->context->CopyResource(texture, buffer);

			D3D11_MAPPED_SUBRESOURCE resource;
			com_assert(
				graphics->context->Map(texture, 0, D3D11_MAP_READ, 0, &resource)
				);

			output->width = texDesc.Width;
			output->height = texDesc.Height;
			output->format = ImageFormat::bgra;
			const uint pitch = output->width * 4;

			output->alloc();

			uint8_t * src = static_cast<uint8_t *>(resource.pData);
			uint8_t * dst = output->data;

			for(uint y = 0; y < output->height; ++y, src += resource.RowPitch, dst += pitch)
				memcpy(dst, src, pitch);

			graphics->context->Unmap(texture, 0);
		}

		void UISurface::apply() const
		{
		#ifdef USE_3D
			graphics->context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
		#else
			graphics->context->OMSetRenderTargets(1, &renderTargetView, nullptr);
		#endif
		}

		void UISurface::onUIResize(Handle<UIResizeMessage> & msg, UISpace & space)
		{
			if(msg->width == 0 || msg->height == 0)
				return;

			viewport.set(msg->width, msg->height);
			graphics->updateUniform<Uniforms::Viewport>(FloatSize{viewport.size()});

			releaseRenderTarget();

			com_assert(
				swapChain->ResizeBuffers(0, viewport.width(), viewport.height(), DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH)
				);

			createRenderTarget();

			space.invalidate();
		}

		void UISurface::onUIFullscreen(Handle<UIFullscreenMessage> & msg, UISpace & space)
		{
			createSwapChain(msg->fullscreen);
		}

		void UISurface::onUIDestroy(Handle<UIDestroyMessage> & msg, UISpace & space)
		{
			swapChain->SetFullscreenState(FALSE, nullptr);
		}

		DepthBufferSurface::DepthBufferSurface(D3DGraphics * graphics, const IntSize & size) : D3DSurface(graphics, size)
		{
			createDepthStencil();
		}

		void DepthBufferSurface::apply() const
		{
		#ifdef USE_3D
			graphics->context->OMSetRenderTargets(1, nullptr, depthStencilView);
		#else
			graphics->context->OMSetRenderTargets(1, nullptr, nullptr);
		#endif
		}

		TextureSurface::TextureSurface(D3DGraphics * graphics, const IntSize & size) : D3DSurface(graphics, size)
		{}

		void TextureSurface::apply() const
		{}

		D3DSurface::D3DSurface(D3DGraphics * graphics, const IntSize & size) : Surface(size), graphics(graphics) {}

		D3DSurface::~D3DSurface() {}

		void D3DSurface::clear() const
		{
			auto cs = hold(graphics->colorState(), graphics->clearColor());
			graphics->rectangle(graphics->clipRect());

			//graphics->context->ClearRenderTargetView(renderTargetView, clearColor);
		#ifdef USE_3D
			graphics->context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		#endif
		}

		void D3DSurface::createDepthStencil()
		{
			ComHandle<ID3D11Texture2D> depthStencil;

			D3D11_TEXTURE2D_DESC descDepth;
			ZeroMemory(&descDepth, sizeof(descDepth));
			descDepth.Width = viewport.width();
			descDepth.Height = viewport.height();
			descDepth.MipLevels = 1;
			descDepth.ArraySize = 1;
			descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			descDepth.SampleDesc.Count = 1;
			descDepth.SampleDesc.Quality = 0;
			descDepth.Usage = D3D11_USAGE_DEFAULT;
			descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			descDepth.CPUAccessFlags = 0;
			descDepth.MiscFlags = 0;

			com_assert(
				graphics->device->CreateTexture2D(&descDepth, NULL, &depthStencil)
				);

			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
			ZeroMemory(&descDSV, sizeof(descDSV));
			descDSV.Format = descDepth.Format;
			descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			descDSV.Texture2D.MipSlice = 0;

			com_assert(
				graphics->device->CreateDepthStencilView(depthStencil, &descDSV, &depthStencilView), "Can't create depth stencil view"
				);
		}

		D3DUniformAdapter::D3DUniformAdapter(D3DGraphics * graphics, ShaderType shader, int index, size_t size) : graphics(graphics), index(index)
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));

			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = static_cast<uint>(size);
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
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

		void D3DUniformAdapter::update(const void * data)
		{
			graphics->context->UpdateSubresource(buffer, 0, NULL, data, 0, 0);
		}
	}
}