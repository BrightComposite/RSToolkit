//---------------------------------------------------------------------------

#include <opengl/opengl.h>
#include <opengl/mesh.h>
#include <opengl/shader.h>

#include <iostream>

//---------------------------------------------------------------------------

namespace asd
{
    namespace opengl
    {
        driver::driver(const configuration & config) : config(config) {
            // register_method(opengl::draw_mesh);
        }
    }

    namespace gfx
    {
        driver_context<opengl::driver>::driver_context(opengl::driver & d) : _driver(d) {}

        driver_context<opengl::driver>::~driver_context() {}

        void driver_context<opengl::driver>::check_for_errors() {
            auto error = glGetError();

            if (error != GL_NO_ERROR) {
                throw context_creation_exception("opengl error, code: ", String::hex(error));
            }
        }

        /*
        handle<image> GLGraphics::createImage(const image_data & data) {
            return handle<GLImage>(this, data);
        }

        handle<vertex_buffer> GLGraphics::createVertexBuffer(vertex_layout * layout, const vertex_data & data) {
            return handle<GLVertexBuffer>(this, layout, data);
        }

        handle<mesh_builder> GLGraphics::createMesh() {
            return handle<mesh_builder>(this);
        }

        handle<UniformAdapter> & GLGraphics::init(handle<UniformAdapter> & adapter, const char * name, ShaderType shader, int index, size_t size) {
            _uniformBindings.emplace_back(name, index);

            for(auto & p : _shaderPrograms) {
                auto id = cast::as<GLShaderProgram>(valueof(p))->id;
                auto block = glGetUniformBlockIndex(id, name);

                if(block != GL_INVALID_INDEX)
                    glUniformBlockBinding(id, block, index);
            }

            checkForErrors();

            adapter = handle<GLUniformAdapter>(this, shader, index, static_cast<uint>(size));
            return adapter;
        }

        handle<asd::surface> GLGraphics::createSurface(ui_space * space) {
            return handle<UISurface>(this, space);
        }

        handle<TextureSurface> GLGraphics::createSurface(const int_size & size) {
            return handle<GLTextureSurface>(this, size);
        }

        void GLGraphics::addShaderProgram(const string & id, vertex_layout * layout, ShaderCodeSet & codeSet) {
            _shaderPrograms[id] = handle<GLShaderProgram>(this, id, layout, codeSet);
        }

        void GLGraphics::present() const {
            _surface->present();
        }

        void GLGraphics::clip(const int_rect & rect) {
            if(_clipRect == rect) {
                return;
            }

            _clipRect = rect;
            glScissor(_clipRect.left, surface()->height() - _clipRect.bottom, _clipRect.width(), _clipRect.height());
        }*/
    }
}
