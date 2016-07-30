//---------------------------------------------------------------------------

#include <opengl/OpenGL3_3.h>
#include <ui/UISpace.h>

#include <iostream>

//---------------------------------------------------------------------------

namespace Rapture
{
	namespace OpenGL
	{
		GLSurface::GLSurface(GLGraphics * graphics, const IntSize & size) : Surface(size), _graphics(graphics) {}
		GLSurface::~GLSurface() {}

		void GLSurface::clear() const
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void GLSurface::present() const
		{

		}

		void GLSurface::createDepthStencil()
		{
			
		}

		RenderTargetSurface::RenderTargetSurface(GLGraphics * graphics, const IntSize & size) : GLSurface(graphics, size) {}
		RenderTargetSurface::~RenderTargetSurface() {}

		void RenderTargetSurface::apply() const
		{
			wglMakeCurrent(_deviceCtx, _graphics->context());
			glViewport(0, 0, width(), height());
		}

		DepthSurface::DepthSurface(GLGraphics * graphics, const IntSize & size) : GLSurface(graphics, size)
		{
			createDepthStencil();
		}

		void DepthSurface::apply() const
		{
			wglMakeCurrent(_deviceCtx, _graphics->context());
			glViewport(0, 0, width(), height());
		}

		void DepthSurface::clear() const
		{
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		UISurface::UISurface(GLGraphics * graphics, UISpace * space) : RenderTargetSurface(graphics, space->size()), _space(space)
		{
			connect(this, &UISurface::onUIResize, *_space);
			connect(this, &UISurface::onUIFullscreen, *_space);
			connect(this, &UISurface::onUIDestroy, *_space);

			createRenderTarget();
		}

		void UISurface::createRenderTarget()
		{
			_deviceCtx = ::GetDC(_space->handle());
			GLGraphics::setPixelFormat(_deviceCtx);

			if(wglMakeCurrent(_deviceCtx, _graphics->context()) == GL_FALSE)
			{
				switch(GetLastError())
				{
					case ERROR_INVALID_VERSION_ARB:
						throw Exception("Can't create render context! Invalid version");
					case ERROR_INVALID_PROFILE_ARB:
						throw Exception("Can't create render context! Invalid profile");
				}

				throw Exception("Can't create render context!");
			}
		}

		void UISurface::releaseRenderTarget()
		{
			if(_graphics->surface() == this)
				wglMakeCurrent(nullptr, nullptr);

			::DeleteDC(_deviceCtx);
			_deviceCtx = nullptr;
		}
		
		void UISurface::present() const
		{
			glFlush();
			SwapBuffers(_deviceCtx);
		}

		void UISurface::requestData(ImageData * output) const
		{
			throw Exception("'requestData' method for OpenGL UISurface is not implemente yet!");

			if(output == nullptr)
				throw Exception("Output image buffer should be not-null!");

			// TODO catch data here:

			void * data;
			uint width, height;
			uint rowPitch;

			// ---------------------

			output->area = {width, height};
			output->format = ImageFormat::bgra;
			const uint pitch = output->width() * 4;

			output->alloc();

			const uint8_t * src = static_cast<const uint8_t *>(data);
			uint8_t * dst = output->ptr;

			for(uint y = 0; y < output->height(); ++y, src += rowPitch, dst += pitch)
				memcpy(dst, src, pitch);

		}

		void UISurface::resize()
		{
			glViewport(0, 0, width(), height());
			_graphics->updateUniform<Uniforms::Viewport>(FloatSize {size()});
		}

		void UISurface::onUIResize(Handle<UIResizeMessage> & msg, UISpace & space)
		{
			if(msg->width == 0 || msg->height == 0)
				return;

			setSize({msg->width, msg->height});
			space.invalidate();
		}

		void UISurface::onUIFullscreen(Handle<UIFullscreenMessage> & msg, UISpace & space)
		{
			
		}

		void UISurface::onUIDestroy(Handle<UIDestroyMessage> & msg, UISpace & space)
		{
			if(_graphics->surface() == this)
				_graphics->bind(null<Surface>());
		}

		TextureSurface::TextureSurface(GLGraphics * graphics, const IntSize & size, Handle<Image> & image) : RenderTargetSurface(graphics, size), _texture(graphics, size.x, size.y)
		{
			// TODO create texture render context
			image = _texture;
		}

		void TextureSurface::clear() const
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void TextureSurface::requestData(ImageData * data) const
		{
			_texture->requestData(data);
		}
	}
}

//---------------------------------------------------------------------------
