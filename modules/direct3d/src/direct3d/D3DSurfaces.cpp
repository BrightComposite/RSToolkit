//---------------------------------------------------------------------------

#include <direct3d/Direct3D11.h>

#include <ui/UISpace.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	namespace Direct3D
	{
		D3DSurface::D3DSurface(D3DGraphics * graphics, const IntSize & size) : Surface(size), _graphics(graphics) {}
		D3DSurface::~D3DSurface() {}

		void D3DSurface::clear() const
		{
			auto cs = hold(_graphics->colorState(), _graphics->clearColor());
			_graphics->rectangle(_graphics->clipRect());

			_graphics->context->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		}

		void D3DSurface::present() const
		{

		}

		void D3DSurface::createDepthStencil()
		{
			ComHandle<ID3D11Texture2D> depthStencil;

			D3D11_TEXTURE2D_DESC descDepth;
			ZeroMemory(&descDepth, sizeof(descDepth));
			descDepth.Width = width();
			descDepth.Height = height();
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
				_graphics->device->CreateTexture2D(&descDepth, NULL, &depthStencil)
			);

			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
			ZeroMemory(&descDSV, sizeof(descDSV));
			descDSV.Format = descDepth.Format;
			descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			descDSV.Texture2D.MipSlice = 0;

			com_assert(
				_graphics->device->CreateDepthStencilView(depthStencil, &descDSV, &_depthStencilView), "Can't create depth stencil view"
			);
		}

		RenderTargetSurface::RenderTargetSurface(D3DGraphics * graphics, const IntSize & size) : D3DSurface(graphics, size) {}
		RenderTargetSurface::~RenderTargetSurface() {}

		void RenderTargetSurface::apply() const
		{
			_graphics->context->OMSetRenderTargets(1, &_renderView, _depthStencilView);

			D3D11_VIEWPORT vp;
			vp.Width = static_cast<float>(width());
			vp.Height = static_cast<float>(height());
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;

			_graphics->context->RSSetViewports(1, &vp);
		}

		DepthSurface::DepthSurface(D3DGraphics * graphics, const IntSize & size) : D3DSurface(graphics, size)
		{
			createDepthStencil();
		}

		void DepthSurface::apply() const
		{
			_graphics->context->OMSetRenderTargets(1, nullptr, _depthStencilView);
		}

		void DepthSurface::clear() const
		{
			_graphics->context->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		}

		UISurface::UISurface(D3DGraphics * graphics, UISpace * space) : RenderTargetSurface(graphics, space->size()), _space(space)
		{
			createSwapChain(false);
			ZeroMemory(&_presentParams, sizeof(_presentParams));

			connect(this, &UISurface::onUIResize, *_space);
			connect(this, &UISurface::onUIFullscreen, *_space);
			connect(this, &UISurface::onUIDestroy, *_space);
		}

		void UISurface::createSwapChain(bool fullscreen)
		{
			releaseRenderTarget();

			ComHandle<IDXGISwapChain> sc;

			DXGI_SWAP_CHAIN_DESC sd;
			ZeroMemory(&sd, sizeof(sd));

			auto & md = sd.BufferDesc;

			md.Width = _space->width();
			md.Height = _space->height();
			md.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

			if(fullscreen)
			{
				md.RefreshRate.Numerator = 60;
				md.RefreshRate.Denominator = 1;
			}

			if(_swapChain != nullptr)
			{
				if(fullscreen)
				{
					ComHandle<IDXGIOutput> output;
					com_assert(_swapChain->GetContainingOutput(&output));

					output->FindClosestMatchingMode(&md, &md, _graphics->device);
					//findPreferredMode(output, md);
				}
				else
					_swapChain->SetFullscreenState(FALSE, nullptr);

				_swapChain = nullptr;
			}

			sd.BufferCount = 1;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.OutputWindow = _space->handle();
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.Windowed = TRUE;
			sd.Flags = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			//sd.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

			com_assert(
				_graphics->dxgiFactory->CreateSwapChain(_graphics->device, &sd, &sc)
			);

			sc.queryInterface(_swapChain);
			createRenderTarget();

			if(fullscreen)
				_swapChain->SetFullscreenState(TRUE, nullptr);
		}

		void UISurface::createRenderTarget()
		{
			createDepthStencil();

			{
				ComHandle<ID3D11Texture2D> buffer;

				com_assert(
					_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), buffer.pointer())
				);

				com_assert(
					_graphics->device->CreateRenderTargetView(buffer, nullptr, &_renderView)
				);
			}

			apply();
			_graphics->bind(this);
		}

		void UISurface::releaseRenderTarget()
		{
			if(_renderView != nullptr || _depthStencilView != nullptr)
			{
				if(_graphics->surface() == this)
				{
					_graphics->context->OMSetRenderTargets(0, 0, 0);
					_graphics->context->Flush();
				}

				_renderView = nullptr;
				_depthStencilView = nullptr;
			}
		}

		void UISurface::findPreferredMode(const ComHandle<IDXGIOutput> & output, DXGI_MODE_DESC & mode)
		{
			uint num = 0;
			output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &num, nullptr);

			owned_data<DXGI_MODE_DESC> descs(num);
			output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &num, descs.ptr);

			for(auto & desc : descs)
			{
				if(desc.RefreshRate.Numerator / desc.RefreshRate.Denominator > 50)
				{
					if(desc.Width >= mode.Width || desc.Height >= mode.Height)
					{
						mode = desc;
						return;
					}
				}
			}
		}

		void UISurface::present() const
		{
			_swapChain->Present(0, 0);
			//_graphics->context->DiscardView(_renderView);
			apply();
		}

		void UISurface::requestData(ImageData * output) const
		{
			if(output == nullptr)
				throw Exception("Output image buffer should be not-null!");

			ComHandle<ID3D11Texture2D> buffer;

			com_assert(
				_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), buffer.pointer())
			);

			ComHandle<ID3D11Texture2D> texture;
			D3D11_TEXTURE2D_DESC texDesc;
			buffer->GetDesc(&texDesc);

			texDesc.BindFlags = 0;
			texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			texDesc.Usage = D3D11_USAGE_STAGING;

			com_assert(
				_graphics->device->CreateTexture2D(&texDesc, nullptr, &texture)
			);

			_graphics->context->CopyResource(texture, buffer);

			D3D11_MAPPED_SUBRESOURCE resource;
			com_assert(
				_graphics->context->Map(texture, 0, D3D11_MAP_READ, 0, &resource)
			);

			output->area = {texDesc.Width, texDesc.Height};
			output->format = ImageFormat::bgra;
			const uint pitch = output->width() * 4;

			output->alloc();

			uint8_t * src = static_cast<uint8_t *>(resource.pData);
			uint8_t * dst = output->ptr;

			for(uint y = 0; y < output->height(); ++y, src += resource.RowPitch, dst += pitch)
				memcpy(dst, src, pitch);

			_graphics->context->Unmap(texture, 0);
		}

		void UISurface::resize()
		{
			releaseRenderTarget();

			com_assert(
				_swapChain->ResizeBuffers(0, _space->width(), _space->height(), DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH)
			);

			createRenderTarget();
		}

		void UISurface::onUIResize(Handle<UIResizeMessage> & msg, UISpace & space)
		{
			if(msg->width == 0 || msg->height == 0)
				return;

			setSize({msg->width, msg->height});
			_graphics->updateUniform<Uniforms::Viewport>(FloatSize {size()});

			space.invalidate();
		}

		void UISurface::onUIFullscreen(Handle<UIFullscreenMessage> & msg, UISpace & space)
		{
			createSwapChain(msg->fullscreen);
		}

		void UISurface::onUIDestroy(Handle<UIDestroyMessage> & msg, UISpace & space)
		{
			_swapChain->SetFullscreenState(FALSE, nullptr);

			if(_graphics->surface() == this)
				_graphics->bind(null<Surface>());
		}

		TextureSurface::TextureSurface(D3DGraphics * graphics, const IntSize & size, Handle<Image> & image) : RenderTargetSurface(graphics, size), _texture(graphics, size.x, size.y)
		{
			com_assert(
				_graphics->device->CreateRenderTargetView(_texture->_handle, nullptr, &_renderView)
			);

			createDepthStencil();
			image = _texture;
		}

		void TextureSurface::clear() const
		{
			_graphics->context->ClearRenderTargetView(_renderView, _graphics->clearColor());
			_graphics->context->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		}

		void TextureSurface::requestData(ImageData * data) const
		{
			_texture->requestData(data);
		}
	}
}

//---------------------------------------------------------------------------
