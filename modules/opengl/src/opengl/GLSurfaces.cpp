//---------------------------------------------------------------------------

#include <opengl/GLObjects.h>
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

		UISurface::UISurface(GLGraphics * graphics, UISpace * space) : GLSurface(graphics, space->size()), _space(space)
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

		void UISurface::apply() const
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			wglMakeCurrent(_deviceCtx, _graphics->context());
			glViewport(0, 0, width(), height());
		}

		void UISurface::present() const
		{
			SwapBuffers(_deviceCtx);
		}

		void UISurface::requestData(ImageData * output) const
		{
			throw Exception("'requestData' method for OpenGL UISurface is not implemented yet!");

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

		GLTextureSurface::GLTextureSurface(GLGraphics * graphics, const IntSize & size) : TextureSurface(size), _graphics(graphics)
		{
			glGenFramebuffers(1, &id);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);

			glGenRenderbuffers(1, &depthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width(), height());

			glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}

		GLTextureSurface::~GLTextureSurface()
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glDeleteRenderbuffers(1, &depthBuffer);
			glDeleteFramebuffers(1, &id);
		}

		void GLTextureSurface::addBuffer(Handle<Texture> & texture, ImageFormat format)
		{
			if(_textures.size() == 16)
				throw Exception("Number of buffers shouldn't be > 16");

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);

			auto & t = *_textures.emplace(_textures.end(), _graphics, width(), height(), format);

			GLenum attachment = GL_COLOR_ATTACHMENT0 + _textures.size() - 1;
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, t->_target, t->_id, 0);
			buffers.push_back(attachment);

			if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				throw Exception("Can't update framebuffer!");

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

			texture = t;

			_graphics->checkForErrors();
		}

		void GLTextureSurface::apply() const
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
			glViewport(0, 0, width(), height());

			glDrawBuffers(buffers.size(), buffers.data());
		}

		void GLTextureSurface::clear() const
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void GLTextureSurface::present() const
		{

		}

		void GLTextureSurface::requestData(ImageData * data) const
		{
			_textures[0]->requestData(data);
		}
	}
}

//---------------------------------------------------------------------------
