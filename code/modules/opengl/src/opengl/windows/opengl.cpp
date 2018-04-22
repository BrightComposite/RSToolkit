//---------------------------------------------------------------------------

#include <opengl/opengl.h>
#include <opengl/mesh.h>
#include <opengl/shader.h>
#include <opengl/uniform.h>

#include <iostream>

//---------------------------------------------------------------------------

namespace asd
{
    namespace opengl
    {
        driver::driver(const configuration & config) : config(config) {
            register_component<gfx3d::uniform::component, uniform_component>();
        }
    }

//---------------------------------------------------------------------------

    namespace gfx
    {
        driver_context<opengl::driver>::driver_context(opengl::driver & d) : _driver(d) {}

        driver_context<opengl::driver>::~driver_context() {}

        void driver_context<opengl::driver>::check_for_errors() {
            GLenum error = glGetError();

            if (error != GL_NO_ERROR) {
                throw std::runtime_error("opengl error, code: " + String::hex(error));
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

    namespace opengl
    {
        void GLAPIENTRY glDebugCallbackFunc(uint source, uint type, uint id, uint severity, int length, const char * message, const void * userParam) {
            string sev, t, src;

            switch (severity) {
            case GL_DEBUG_SEVERITY_LOW:
                sev = "low";
                break;

            case GL_DEBUG_SEVERITY_MEDIUM:
                sev = "medium";
                break;

            case GL_DEBUG_SEVERITY_HIGH:
                sev = "high";
                break;

            case GL_DEBUG_SEVERITY_NOTIFICATION:
                return;
                //sev = "notification";
                //break;

            default:
                sev = String::hex(severity);
            }

            switch (type) {
            case GL_DEBUG_TYPE_ERROR:
                t = "error";
                break;

            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                t = "deprecated behavior";
                break;

            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                t = "undefined behavior";
                break;

            case GL_DEBUG_TYPE_PORTABILITY:
                t = "portability";
                break;

            case GL_DEBUG_TYPE_PERFORMANCE:
                t = "performance";
                break;

            case GL_DEBUG_TYPE_OTHER:
                t = "other";
                break;

            case GL_DEBUG_TYPE_MARKER:
                t = "marker";
                break;

            case GL_DEBUG_TYPE_PUSH_GROUP:
                t = "push group";
                break;

            case GL_DEBUG_TYPE_POP_GROUP:
                t = "pop group";
                break;

            default:
                t = String::assemble("unknown (", String::hex(type), ")");
            }

            switch (source) {
            case GL_DEBUG_SOURCE_API:
                src = "API";
                break;

            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                src = "window system";
                break;

            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                src = "shader compiler";
                break;

            case GL_DEBUG_SOURCE_THIRD_PARTY:
                src = "3rd party";
                break;

            case GL_DEBUG_SOURCE_APPLICATION:
                src = "application";
                break;

            default:
                src = String::assemble("unknown (", String::hex(source), ")");
            }

            using namespace std;

            cout << "--------------------------------" << endl;
            cout << "OpenGL debug:" << endl;
            cout << "\tgroup: " << t << ", severity: " << sev << ", id: " << hex << showbase << id << dec
                << ", source: " << src << endl;
            cout << message << endl << endl;

            if (severity == GL_DEBUG_SEVERITY_HIGH) {
                throw Exception("GL error!");
            }
        }
    }
}
