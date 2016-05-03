//---------------------------------------------------------------------------

#include <direct3d/Direct3D11.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>

#include <ui/WindowAdapter.h>
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

		implement(Graphics3D::pool);

		implement(D3DModel::quad);
		implement(D3DModel::texquad);

		class FillModeState : public State<FillMode>
		{
		public:
			FillModeState(GraphicContext * ctx) : State<FillMode>(FillMode::Solid), ctx(ctx) {}

		protected:
			virtual void change() override
			{
				if(_state == FillMode::Solid)
					ctx->context->RSSetState(ctx->solidRS);
				else
					ctx->context->RSSetState(ctx->wiredRS);
			}

			GraphicContext * ctx;
		};

		class DepthTestState : public State<bool>
		{
		public:
			DepthTestState(GraphicContext * ctx) : State<bool>(false), ctx(ctx) {}

		protected:
			virtual void change() override
			{
				if(_state)
					ctx->context->OMSetDepthStencilState(ctx->depthState3D, 0);
				else
					ctx->context->OMSetDepthStencilState(ctx->depthState2D, 0);

				ctx->setDepth(1.0f);
			}

			GraphicContext * ctx;
		};

		Graphics3D::Graphics3D()
		{
			setclass(Graphics3D);

			_fillMode = handle<FillModeState>(this);
			_depthTestMode = handle<DepthTestState>(this);
		}

		Graphics3D::~Graphics3D()
		{
			if(context != nullptr)
			{
				context->OMSetRenderTargets(0, 0, 0);
				context->Flush();

				context->ClearState();
			}
		}

		void Graphics3D::initDevice()
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

			setContext(ndevice, ncontext);

		#ifdef DEBUG_DX
			debug.instance = device;
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

		void Graphics3D::initFacilities()
		{
			Shaders::initialize();

			auto & p2 = VertexLayout::get("p2");
			auto & p2t = VertexLayout::get("p2 t");

			D3DModel::quad.init(this, p2, VertexData::quad);
			D3DModel::texquad.init(this, p2t, VertexData::texquad);

			updateBrushState();
		}

		void Graphics3D::freeFacilities()
		{
			D3DModel::quad = nullptr;
			D3DModel::texquad = nullptr;

			Shaders::free();
		}

		void Graphics3D::printInfo() {}
		void Graphics3D::printDebug() {}
		void Graphics3D::checkForErrors() {}

		void Graphics3D::bind(const Handle<Surface> & surface)
		{
			if(_surface == surface)
				return;

			_surface = surface;
			clip(viewport());
			_surface->apply();
		}

		void Graphics3D::bind(const Handle<Texture> & texture, uint index)
		{
			Graphics3D * graphics = Graphics3D::instance();

			if(graphics->textures.size() > index && graphics->textures[index] == texture)
				return;

			graphics->textures[index] = texture;
			texture->apply();
		}

		Handle<Image> Graphics3D::createImage(const ImageData & data) const
		{
			return Handle<D3DImage>(this, data);
		}

		Handle<Figure> Graphics3D::createFigure(const FigureData & data) const
		{
			return Handle<D3DFigure>(this, data);
		}

		Handle<Surface> Graphics3D::createWindowSurface(WindowAdapter * adapter) const
		{
			auto & surface = adapter->surface();

			if(surface != nullptr)
				return surface;

			auto newSurface = handle<WindowSurface>(adapter);

			connect(*adapter, newSurface, &WindowSurface::onWindowResize);
			connect(*adapter, newSurface, &WindowSurface::onWindowFullscreen);
			connect(*adapter, newSurface, &WindowSurface::onWindowClose);

			return newSurface;
		}

		void Graphics3D::updateBrushState()
		{
			updateUniform<Uniforms::BrushOptions>({color(), lineSize()});
		}

		void Graphics3D::present() const
		{
			_surface->present();
		}

		void Graphics3D::clip(const IntRect & rect)
		{
			if(_clipRect == rect)
				return;

			RectAdapter r;

			_clipRect = rect;
			context->RSSetScissorRects(1, &r.set(_clipRect));
		}

		void Graphics3D::rectangle(const IntRect & rect)
		{
			auto fm = hold(_fillMode, FillMode::Solid);

			FloatPoint t;
			FloatSize s;

			ScreenCoord::toRel(rect, viewport(), t, s);
			updateUniform<Uniforms::Area>({t, s, _depth});

			if(fm.old == FillMode::Solid)
				SimpleTechnique::rectangle->apply();
			else
				SimpleTechnique::wired_rectangle->apply();

			draw(D3DModel::quad);
		}

		void Graphics3D::ellipse(const IntRect & rect)
		{
			auto fm = hold(_fillMode, FillMode::Solid);

			FloatPoint t;
			FloatSize s;

			ScreenCoord::toSq(rect, viewport(), t, s);
			updateUniform<Uniforms::Area>({t, s, _depth});

			if(fm.old == FillMode::Solid)
				SimpleTechnique::ellipse->apply();
			else
				SimpleTechnique::wired_ellipse->apply();

			draw(D3DModel::texquad);
		}

		void Graphics3D::rectangle(const SqRect & rect)
		{
			auto fm = hold(_fillMode, FillMode::Solid);

			FloatPoint t;
			FloatSize s;

			ScreenCoord::toRel(rect, viewport(), t, s);
			updateUniform<Uniforms::Area>({t, s, _depth});

			if(fm.old == FillMode::Solid)
				SimpleTechnique::rectangle->apply();
			else
				SimpleTechnique::wired_rectangle->apply();

			draw(D3DModel::texquad);
		}

		void Graphics3D::ellipse(const SqRect & rect)
		{
			auto fm = hold(_fillMode, FillMode::Solid);
			auto s = rect.size();

			updateUniform<Uniforms::Area>({rect.pos() + s * 0.5f, s, _depth});

			if(fm.old == FillMode::Solid)
				SimpleTechnique::ellipse->apply();
			else
				SimpleTechnique::wired_ellipse->apply();

			draw(D3DModel::texquad);
		}

		void Graphics3D::draw(const Figure * figure, const IntRect & bounds)
		{
			if(figure->graphics != this)
				return;
			
			FloatPoint t;
			FloatSize s;

			ScreenCoord::toRel(bounds, viewport(), t, s);

			updateUniform<Uniforms::Model>(FloatTransform(t, s, _depth));
			SimpleTechnique::figure->apply();

			draw(static_cast<const D3DFigure *>(figure)->model);
		}

		void Graphics3D::draw(const Figure * figure, const FloatTransform & transform)
		{
			if(figure->graphics != this)
				return;

			const auto & v = viewport();

			updateUniform<Uniforms::Model>(transform * FloatScaling {1.0f, v.invratio(), 1.0f, 1.0f});
			SimpleTechnique::figure->apply();

			draw(static_cast<const D3DFigure *>(figure)->model);
		}

		void Graphics3D::draw(const Model * model)
		{
			if(model->graphics != this)
				return;

			draw(static_cast<const D3DModel *>(model));
		}

		void Graphics3D::draw(const D3DModel * model)
		{
			if(model->buffer != vbuffer)
			{
				vbuffer = model->buffer;

				uint stride = vbuffer->vil->stride;
				uint offset = 0;

				context->IASetVertexBuffers(0, 1, &vbuffer->handle, &stride, &offset);
				context->IASetPrimitiveTopology(vbuffer->topology);
			}

			if(model notinstanceof (D3DIndexedModel))
			{
				context->Draw(vbuffer->size - model->verticesLocation, model->verticesLocation);
				return;
			}

			auto * m = static_cast<const D3DIndexedModel *>(model);

			if(m->indices != ibuffer)
			{
				ibuffer = m->indices;
				context->IASetIndexBuffer(ibuffer->handle, DXGI_FORMAT_R16_UINT, 0);
			}

			context->DrawIndexed(ibuffer->size - m->indicesLocation, m->indicesLocation, model->verticesLocation);
		}

		void Graphics3D::draw(const Image * image, int x, int y)
		{
			draw(image, IntRect{x, y, static_cast<int>(x + image->width()), static_cast<int>(y + image->height())});
		}

		void Graphics3D::draw(const Image * image, const IntRect & rect)
		{
			if(image->graphics() != this)
				return;

			auto img = static_cast<const D3DImage *>(image);
			img->apply();

			FloatPoint t;
			FloatSize s;

			ScreenCoord::toRel(rect, viewport(), t, s);
			updateUniform<Uniforms::Area>({t, s, _depth});

			SimpleTechnique::image->apply();

			draw(D3DModel::texquad);
		}

		void Graphics3D::draw(const Image * image, const SqRect & rect)
		{
			if(image->graphics() != this)
				return;

			auto img = static_cast<const D3DImage *>(image);
			img->apply();

			FloatPoint t;
			FloatSize s;

			ScreenCoord::toRel(rect, viewport(), t, s);
			updateUniform<Uniforms::Area>({t, s, _depth});

			SimpleTechnique::image->apply();

			draw(D3DModel::texquad);
		}

		void Graphics3D::draw(const Symbol * symbol, int x, int y)
		{
			if(symbol->graphics() != this)
				return;

			auto img = static_cast<const D3DImage *>(symbol->image());

			if(img == nullptr)
				return;

			img->apply();

			IntRect rect;
			rect.setPlacement(x + symbol->left(), y + symbol->top(), img->_width, img->_height);

			FloatPoint t;
			FloatSize s;

			ScreenCoord::toRel(rect, viewport(), t, s);
			updateUniform<Uniforms::Area>({t, s, _depth});

			SimpleTechnique::text->apply();

			draw(D3DModel::texquad);
		}

		void Graphics3D::draw(const Image * image, int x, int y, int width, int height)
		{
			draw(image, IntRect{x, y, x + width, y + height});
		}

		FxTechnique::FxTechnique(const Handle<VertexLayout> & vil) : ctx(Graphics3D::instance()), vil(vil) {}

		VertexBuffer::VertexBuffer(const Handle<VertexLayout> & vil, const VertexData & vd, D3D_PRIMITIVE_TOPOLOGY topology) : ctx(Graphics3D::instance()), topology(topology), vil(vil), size(static_cast<uint>(vd.size / vil->stride))
		{
			if(vd.size % vil->stride != 0)
				throw Exception("Size of vertex buffer doesn't matches its vertex input layout: \"", vil->key, "\"");

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
				ctx->device->CreateBuffer(&bd, &sdata, &handle),
				"Can't create vertex buffer!"
				);
		}

		IndexBuffer::IndexBuffer(const VertexIndices & indices) : ctx(Graphics3D::instance()), size(static_cast<uint>(indices.size()))
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
				ctx->device->CreateBuffer(&bd, &sdata, &handle),
				"Can't create index buffer!"
				);
		}

		CommonShader::Shader() : ctx(Graphics3D::instance()) {}

		void CommonShader::read(const String & filename, ShaderCode & out)
		{
			ComHandle<ID3DBlob> code;
			com_assert(
				D3DReadFileToBlob(widen(filename).c_str(), &code),
				"Can't read shader from file: ", filename
				);

			out.data = code->GetBufferPointer();
			out.size = code->GetBufferSize();
		}

		void CommonShader::compile(const String & filename, const String & entrance, const String & shaderModel, ShaderCode & out)
		{
			ComHandle<ID3DBlob> code;
			DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
		#ifdef DEBUG_DX
			flags |= D3DCOMPILE_DEBUG;
		#endif
			ComHandle<ID3DBlob> error;
			HRESULT hr = D3DCompileFromFile(widen(filename).c_str(), nullptr, nullptr, entrance, shaderModel, flags, 0, &code, &error);

			com_assert(hr, (char *)error->GetBufferPointer());

			out.data = code->GetBufferPointer();
			out.size = code->GetBufferSize();
		}

		//#define COMPILE_SHADERS

		VertexShader::Shader(const Handle<FxTechnique> & technique, const String & path, ShaderCodeState state)
		{
			ShaderCode code;

			switch(state)
			{
				case ShaderCodeState::Raw:
					compile(path + "_vs.fx", "main", "vs_4_0", code);
					break;

				case ShaderCodeState::Compiled:
					read(path + "_vs.cso", code);
					break;

				case ShaderCodeState::Embedded:
					code = Shaders::getCode(path, ShaderType::Vertex);
					break;
			}

			init(technique, code);
		}

		VertexShader::Shader(const Handle<FxTechnique> & technique, const Handle<ShaderCode> & code)
		{
			init(technique, *code);
		}

		void VertexShader::apply() const
		{
			ctx->context->IASetInputLayout(layout);
			ctx->context->VSSetShader(id, nullptr, 0);
		}

		void VertexShader::init(const Handle<FxTechnique> & technique, const ShaderCode & code)
		{
			HRESULT hr = S_OK;

			auto & ctx = technique->ctx;

			com_assert(
				ctx->device->CreateVertexShader(code.data, code.size, nullptr, &id),
				"Can't create vertex shader!"
				);

			auto & vlayout = technique->vil->layout;

			com_assert(
				ctx->device->CreateInputLayout(vlayout.data(), static_cast<uint>(vlayout.size()), code.data, code.size, &layout),
				"Can't create vertex input layout for shader"
				);
		}

		PixelShader::Shader(const Handle<FxTechnique> & technique, const String & path, ShaderCodeState state)
		{
			ShaderCode code;

			switch(state)
			{
				case ShaderCodeState::Raw:
					compile(path + "_ps.fx", "main", "ps_4_0", code);
					break;

				case ShaderCodeState::Compiled:
					read(path + "_ps.cso", code);
					break;

				case ShaderCodeState::Embedded:
					code = Shaders::getCode(path, ShaderType::Pixel);
					break;
			}

			init(technique, code);
		}

		PixelShader::Shader(const Handle<FxTechnique> & technique, const Handle<ShaderCode> & code)
		{
			init(technique, *code);
		}

		void PixelShader::apply() const
		{
			ctx->context->PSSetShader(id, nullptr, 0);
		}

		void PixelShader::init(const Handle<FxTechnique> & technique, const ShaderCode & code)
		{
			com_assert(
				technique->ctx->device->CreatePixelShader(code.data, code.size, nullptr, &id),
				"Can't create pixel shader!"
				);
		}

		D3DImage::D3DImage(const GraphicContext * ctx, const ImageData & data) : Image(ctx, data), _ctx(ctx)
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
				ctx->device->CreateTexture2D(&texDesc, &texInitData, &tex)
				);

			com_assert(
				ctx->device->CreateShaderResourceView(tex, nullptr, &_handle)
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
				ctx->device->CreateSamplerState(&sampDesc, &_state)
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

		D3DFigure::D3DFigure(const Graphics3D * graphics, const FigureData & data) : Figure(graphics, data)
		{
			vector<FloatPoint> points;
			auto & p = data.points;

			for(int i = 0; i < p.size() - 3; i += 2)
			{
				points.push_back(p[i]);
				points.push_back(p[i + 1]);
				points.push_back(p[i + 2]);
			}

			points.push_back(p[p.size() - 2]);
			points.push_back(p[p.size() - 1]);
			points.push_back(p[0]);

			model = handle<D3DModel>(graphics, VertexLayout::get("p2"), points);
		}

	#define USE_3D

		WindowSurface::WindowSurface(WindowAdapter * adapter) : D3DSurface(adapter->size()), adapter(adapter)
		{
			createSwapChain(false);
			ZeroMemory(&presentParams, sizeof(presentParams));
		}

		WindowSurface::~WindowSurface() {}

		void WindowSurface::createSwapChain(bool fullscreen)
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

					output->FindClosestMatchingMode(&md, &md, ctx->device);
					//findPreferredMode(output, md);
				}
				else
					swapChain->SetFullscreenState(FALSE, nullptr);

				swapChain = nullptr;
			}

			sd.BufferCount = 1;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.OutputWindow = adapter->windowHandle();
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.Windowed = TRUE;
			sd.Flags = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			//sd.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

			com_assert(
				ctx->dxgiFactory->CreateSwapChain(ctx->device, &sd, &sc)
				);

			swapChain = sc;
			createRenderTarget();

			if(fullscreen)
				swapChain->SetFullscreenState(TRUE, nullptr);
		}

		void WindowSurface::createRenderTarget()
		{
			createDepthStencil();

			D3D11_VIEWPORT vp;
			vp.Width = static_cast<float>(viewport.width());
			vp.Height = static_cast<float>(viewport.height());
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;

			ctx->context->RSSetViewports(1, &vp);

			{
				ComHandle<ID3D11Texture2D> buffer;

				com_assert(
					swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), buffer.pointer())
					);

				com_assert(
					ctx->device->CreateRenderTargetView(buffer, nullptr, &renderTargetView)
					);
			}

		#ifdef USE_3D
			ctx->context->OMSetRenderTargets(1, &renderTargetView, nullptr);
		#else
			ctx->context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
		#endif
		}

		void WindowSurface::releaseRenderTarget()
		{
			if(renderTargetView != nullptr || depthStencilView != nullptr)
			{
				ctx->context->OMSetRenderTargets(0, 0, 0);
				ctx->context->Flush();
				renderTargetView = nullptr;
				depthStencilView = nullptr;
			}
		}

		void WindowSurface::findPreferredMode(const ComHandle<IDXGIOutput> & output, DXGI_MODE_DESC & mode)
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

		void WindowSurface::present() const
		{
			swapChain->Present(0, 0);
			//ctx->context->DiscardView(renderTargetView);
			apply();
		}

		void WindowSurface::requestData(ImageData * output) const
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
				ctx->device->CreateTexture2D(&texDesc, nullptr, &texture)
				);

			ctx->context->CopyResource(texture, buffer);

			D3D11_MAPPED_SUBRESOURCE resource;
			com_assert(
				ctx->context->Map(texture, 0, D3D11_MAP_READ, 0, &resource)
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

			ctx->context->Unmap(texture, 0);
		}

		void WindowSurface::apply() const
		{
		#ifdef USE_3D
			ctx->context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
		#else
			ctx->context->OMSetRenderTargets(1, &renderTargetView, nullptr);
		#endif
		}

		void WindowSurface::onWindowResize(Handle<WindowResizeMessage> & msg, WindowAdapter & adapter)
		{
			if(msg->width == 0 || msg->height == 0)
				return;

			viewport.set(msg->width, msg->height);
			Graphics3D::instance()->updateUniform<Uniforms::Viewport>(FloatSize{viewport.size()});

			releaseRenderTarget();

			com_assert(
				swapChain->ResizeBuffers(0, viewport.width(), viewport.height(), DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH)
				);

			createRenderTarget();

			adapter.invalidate();
		}

		void WindowSurface::onWindowFullscreen(Handle<WindowFullscreenMessage> & msg, WindowAdapter & adapter)
		{
			createSwapChain(msg->fullscreen);
		}

		void WindowSurface::onWindowClose(Handle<WindowCloseMessage> & msg, WindowAdapter & adapter)
		{
			swapChain->SetFullscreenState(FALSE, nullptr);
		}

		DepthBufferSurface::DepthBufferSurface(const IntSize & size) : D3DSurface(size)
		{
			createDepthStencil();
		}

		void DepthBufferSurface::apply() const
		{
		#ifdef USE_3D
			ctx->context->OMSetRenderTargets(1, nullptr, depthStencilView);
		#else
			ctx->context->OMSetRenderTargets(1, nullptr, nullptr);
		#endif
		}

		TextureSurface::TextureSurface(const IntSize & size) : D3DSurface(size)
		{}

		void TextureSurface::apply() const
		{}

		VertexElement            VertexElement::pos2("p2", "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, sizeof(float[2]));
		VertexElement            VertexElement::pos3("p3", "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, sizeof(float[3]));
		VertexElement          VertexElement::color3("c3", "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, sizeof(float[3]));
		VertexElement          VertexElement::color4("c4", "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, sizeof(float[4]));
		VertexElement             VertexElement::tex("t", "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, sizeof(float[2]));
		VertexElement          VertexElement::normal("n", "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, sizeof(float[3]));
		VertexElement VertexElement::secondaryColor3("s3", "COLOR", 1, DXGI_FORMAT_R32G32B32_FLOAT, sizeof(float[3]));
		VertexElement VertexElement::secondaryColor4("s4", "COLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, sizeof(float[4]));

		D3DSurface::D3DSurface(const IntSize & size) : Rapture::Surface(size), ctx(Graphics3D::instance()) {}

		D3DSurface::~D3DSurface() {}

		void D3DSurface::clear() const
		{
			auto cs = hold(ctx->colorState(), ctx->clearColor());
			ctx->rectangle(ctx->clipRect());

			//ctx->context->ClearRenderTargetView(renderTargetView, clearColor);
		#ifdef USE_3D
			ctx->context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
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
				ctx->device->CreateTexture2D(&descDepth, NULL, &depthStencil)
				);

			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
			ZeroMemory(&descDSV, sizeof(descDSV));
			descDSV.Format = descDepth.Format;
			descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			descDSV.Texture2D.MipSlice = 0;

			com_assert(
				ctx->device->CreateDepthStencilView(depthStencil, &descDSV, &depthStencilView)
				);
		}

		Uniform::Uniform(ShaderType shader, uint index, uint size) : ctx(Graphics3D::instance()), index(index)
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));

			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = size;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = 0;

			com_assert(
				ctx->device->CreateBuffer(&bd, NULL, &buffer)
				);

			switch(shader)
			{
				case ShaderType::Vertex:
					ctx->context->VSSetConstantBuffers(index, 1, &buffer);
					break;

				case ShaderType::Pixel:
					ctx->context->PSSetConstantBuffers(index, 1, &buffer);
					break;
			}
		}

		void Uniform::update(const void * data)
		{
			ctx->context->UpdateSubresource(buffer, 0, NULL, data, 0, 0);
		}
	}
}