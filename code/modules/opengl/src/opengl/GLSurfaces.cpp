//---------------------------------------------------------------------------

#include <opengl/GLObjects.h>
#include <ui/ui_space.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace opengl
	{
		GLSurface::GLSurface(GLGraphics * graphics, const int_size & size) : surface(size), _graphics(graphics) {}
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

		DepthSurface::DepthSurface(GLGraphics * graphics, const int_size & size) : GLSurface(graphics, size)
		{
			createDepthStencil();
		}

		void DepthSurface::apply() const
		{
#if BOOST_OS_WINDOWS
			wglMakeCurrent(_deviceCtx, _graphics->context());
#endif
			glViewport(0, 0, width(), height());
		}

		void DepthSurface::clear() const
		{
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		UISurface::UISurface(GLGraphics * graphics, ui_space * space) : GLSurface(graphics, space->size()), _space(space)
		{
			connect(this, &UISurface::onUIResize, *_space);
			connect(this, &UISurface::onUIFullscreen, *_space);
			connect(this, &UISurface::onUIDestroy, *_space);

			createRenderTarget();
		}

		void UISurface::createRenderTarget()
		{
#if BOOST_OS_WINDOWS
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
#elif BOOST_OS_LINUX
			if(glXMakeCurrent(_graphics->display(), _space->handle(), _graphics->context()) == GL_FALSE) {
				throw Exception("Can't bind render context!");
			}
#endif
		}

		void UISurface::releaseRenderTarget()
		{
#if BOOST_OS_WINDOWS
			if(_graphics->surface() == this)
				wglMakeCurrent(nullptr, nullptr);

			::DeleteDC(_deviceCtx);
			_deviceCtx = nullptr;
#elif BOOST_OS_LINUX
			if(_graphics->surface() == this)
				glXMakeCurrent(_graphics->display(), 0, 0);
#endif
		}

		void UISurface::apply() const
		{
#if BOOST_OS_WINDOWS
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			wglMakeCurrent(_deviceCtx, _graphics->context());
#endif
			glViewport(0, 0, width(), height());
		}

		void UISurface::present() const
		{
#if BOOST_OS_WINDOWS
			SwapBuffers(_deviceCtx);
#elif BOOST_OS_LINUX
			glXSwapBuffers(_graphics->display(), _space->handle());
#endif
		}

		void UISurface::requestData(image_data * output) const
		{
			throw Exception("'requestData' method for opengl UISurface is not implemented yet!");

			if(output == nullptr)
				throw Exception("Output image buffer should be not-null!");

			// TODO catch data here:

			void * data;
			uint width, height;
			uint rowPitch;

			// ---------------------

			output->area = {width, height};
			output->format = image_format::bgra;
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
			_graphics->updateUniform<Uniforms::Viewport>(float_size {size()});
		}

		void UISurface::onUIResize(handle<UIResizeMessage> & msg, ui_space & space)
		{
			if(msg->width == 0 || msg->height == 0)
				return;

			setSize({msg->width, msg->height});
			space.invalidate();
		}

		void UISurface::onUIFullscreen(handle<UIFullscreenMessage> & msg, ui_space & space)
		{
			
		}

		void UISurface::onUIDestroy(handle<UIDestroyMessage> & msg, ui_space & space)
		{
			if(_graphics->surface() == this)
				_graphics->bind(null<surface>());
		}

		GLTextureSurface::GLTextureSurface(GLGraphics * graphics, const int_size & size) : TextureSurface(size), _graphics(graphics)
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

		void GLTextureSurface::addBuffer(handle<Texture> & texture, image_format format)
		{
			if(_textures.size() == 16)
				throw Exception("Number of buffers shouldn't be > 16");

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);

			auto & t = *_textures.emplace(_textures.end(), _graphics, width(), height(), format);

			GLenum attachment = static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + _textures.size() - 1);
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

			glDrawBuffers(static_cast<GLsizei>(buffers.size()), buffers.data());
		}

		void GLTextureSurface::clear() const
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void GLTextureSurface::clear(uint bitmask) const
		{
			array_list<GLenum> b;

			repeat(i, buffers.size()) {
				if(check_flag((1 << i), bitmask))
				{
					b.push_back(static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i));
				}
			}

			glDrawBuffers(static_cast<GLsizei>(b.size()), b.data());
			glClear(GL_COLOR_BUFFER_BIT);
			glDrawBuffers(static_cast<GLsizei>(buffers.size()), buffers.data());
		}

		void GLTextureSurface::resize()
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);

			glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width(), height());

			for(auto & tex : _textures)
				tex->setSize(size());

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}

		void GLTextureSurface::present() const
		{

		}

		void GLTextureSurface::requestData(image_data * data) const
		{
			_textures[0]->requestData(data);
		}
	}
}

//---------------------------------------------------------------------------
