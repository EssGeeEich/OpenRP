#include "gamewindow.h"
#include "lua_ogl_link.h"
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QMessageBox>

namespace LuaApi {
    typedef Lua::lua_exception gl_error;

    class glState {
        Texture m_errorTexture;
        Texture m_textures[8];

        class UniqueSquare {
            QOpenGLBuffer m_buffer;
            QOpenGLShaderProgram m_shader;
            QOpenGLVertexArrayObject m_vao;
            GL_t* m_gl;
            GameWindow* m_gw;
        public:
            UniqueSquare() : m_buffer(QOpenGLBuffer::VertexBuffer), m_shader(), m_vao(), m_gl(nullptr) {}

            void Init(GL_t* gl, GameWindow* gw) {
                if(m_gl)
                    throw gl_error("Invalid operation: OpenGL Entity is being initialized twice!");

                m_gw = gw;
                m_gl = gl;
                static float const square[] = {
                    0.f, 0.f,
                    0.f, 2.f,
                    2.f, 0.f,
                    2.f, 0.f,
                    0.f, 2.f,
                    2.f, 2.f
                };
                if(!m_buffer.create())
                    throw gl_error("Could not create OpenGL Buffer!");
                if(!m_shader.create())
                    throw gl_error("Could not create OpenGL Shader Program!");
                if(!m_buffer.bind())
                    throw gl_error("Could not bind OpenGL Buffer!");
                m_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
                m_buffer.allocate(square, sizeof(square));
                m_shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/quad/vert.vsh");
                m_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/quad/frag.fsh");
                if(!m_shader.link())
                    throw gl_error("Could not link OpenGL Shader Program!");
                if(!m_shader.bind())
                    throw gl_error("Could not bind OpenGL Shader Program!");

                if(!m_vao.create())
                    throw gl_error("Could not create OpenGL VAO!");
                m_vao.bind();
                m_shader.enableAttributeArray(0);
                m_shader.setAttributeBuffer(0, GL_FLOAT, 0, 2);
                m_vao.release();
                m_buffer.release();
                m_shader.release();

            }

            void DrawRaw(double x, double y, double w, double h) {
                if(!m_shader.bind())
                    throw gl_error("Could not bind OpenGL Shader Program!");
                m_vao.bind();
                m_shader.setUniformValue("g_offset", x * 2.0, y * 2.0);
                m_shader.setUniformValue("g_size", w, h);
                m_gl->glDrawArrays(GL_TRIANGLES, 0, 6);
                m_vao.release();
                m_shader.release();
            }

            void DrawCorrected(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h) {
                if(!m_shader.bind())
                    throw gl_error("Could not bind OpenGL Shader Program!");
                QSize winSize = m_gw->size();
                double invSizeX = 1.0 / static_cast<double>(winSize.width());
                double invSizeY = 1.0 / static_cast<double>(winSize.height());

                m_vao.bind();
                m_shader.setUniformValue("g_offset", static_cast<double>(x * 2) * invSizeX,
                                                      static_cast<double>(y * 2) * invSizeY);
                m_shader.setUniformValue("g_size", static_cast<double>(w) * invSizeX,
                                                    static_cast<double>(h) * invSizeY);
                m_gl->glDrawArrays(GL_TRIANGLES, 0, 6);
                m_vao.release();
                m_shader.release();
            }
        } m_uniqueSquare;
    public:
        glState() {}

        void Init() {
            m_errorTexture.load(":/textures/error.png",Lua::Arg<bool>(false));
            if(m_errorTexture.texture())
            {
                m_errorTexture.texture()->setMagnificationFilter(QOpenGLTexture::Nearest);
                m_errorTexture.texture()->setMinificationFilter(QOpenGLTexture::Nearest);
            }
            for(int i = 0; i < 8; ++i)
                m_textures[i] = m_errorTexture;
        }

        void SetTexture(Lua::Arg<Texture> const& tex, std::uint32_t slot) {
            if(slot < 8)
                m_textures[slot] = tex.get_safe(m_errorTexture);
        }

        void Apply() {
            for(int i = 0; i < 8; ++i)
            {
                if(m_textures[i].texture())
                    m_textures[i].texture()->bind(i);
            }
        }
    };



    void RegisterGlobals(GL_t* gl, Lua::State& state)
    {
#define REGFNC(name) state.luapp_add_translated_function( #name , Lua::Transform(*gl, &GL_t::name))
        REGFNC(glEnable);
        REGFNC(glDisable);
        REGFNC(glDepthFunc);
        REGFNC(glClearColor);
        REGFNC(glClear);
    }
}

void GameWindow::glToLua()
{
    LuaApi::RegisterGlobals(this, state);
    if(!m_storage.InitAll(this, this, state))
    {
        hide();
        QMessageBox::critical(nullptr,tr("OpenGL Loading Error"),tr("Error initializing OpenGL State!\nCheck for OpenGL 4.4 support!"));
        CloseToStartupWindow();
        return;
    }
}
