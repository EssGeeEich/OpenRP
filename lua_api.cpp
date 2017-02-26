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
