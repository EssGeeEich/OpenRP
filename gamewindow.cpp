#include "gamewindow.h"
#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include "startupwindow.h"

GameWindow::GameWindow(QWindow* parent, GameMode gamemode, StartupWindow* startupWindow) :
    QOpenGLWindow(QOpenGLWindow::NoPartialUpdate, parent),
    state(Lua::State::create()),
    m_gamemode(gamemode),
    m_startupWindow(startupWindow)
{
    this->setTitle(tr("OpenRP - %1 - %2").arg(QCoreApplication::applicationVersion()).arg(m_gamemode.Name));
    state.luapp_register_metatables();
}

void GameWindow::printContextInformations()
{
    QString glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
    QString glVersion = reinterpret_cast<char const*>(glGetString(GL_VERSION));
    QString glProfile;

    switch(format().profile())
    {
    case QSurfaceFormat::NoProfile:
        glProfile = "(No Profile)";
        break;
    case QSurfaceFormat::CoreProfile:
        glProfile = "(Core Profile)";
        break;
    case QSurfaceFormat::CompatibilityProfile:
        glProfile = "(Compatibility Profile)";
        break;
    default:
        glProfile = "(Invalid Profile)";
        break;
    }

    qDebug() << qPrintable(glType) << qPrintable(glVersion) << qPrintable(glProfile);
}

void GameWindow::initializeGL()
{
    initializeOpenGLFunctions();
    printContextInformations();
    registerLuaFunctions();
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClearDepthf(0.0f);
    if(state.getglobal("startup") != LUA_TFUNCTION)
    {
        state.pop(1);
        return;
    }
    if(state.pcall() != 0)
    {
        hide();
        QMessageBox::critical(nullptr,tr("Error running gamemode %1").arg(m_gamemode.Name),tr("Lua Error while loading window.\nLua Error: %1").arg(QString::fromStdString(state.tostdstring(1))));
        CloseToStartupWindow();
        return;
    }
}

static const luaL_Reg loadedlibs[] = {
    {"_G", luaopen_base},
    {LUA_LOADLIBNAME, luaopen_package},
    {LUA_COLIBNAME, luaopen_coroutine},
    {LUA_TABLIBNAME, luaopen_table},
    {LUA_STRLIBNAME, luaopen_string},
    {LUA_MATHLIBNAME, luaopen_math},
    {LUA_UTF8LIBNAME, luaopen_utf8},
    {NULL, NULL}
};

void GameWindow::registerLuaFunctions()
{
    {
        const luaL_Reg* lib;
        for(lib = loadedlibs; lib->func; lib++) {
            state.requiref(lib->name,lib->func,1);
            state.pop(1);
        }
    }

    for(auto it = m_gamemode.Modules.begin(); it != m_gamemode.Modules.end(); ++it)
    {
        QString str = *it;
        if(str.at(0) != '/')
            str = "gamemode/" + m_gamemode.SubFolder + "/" + *it;
        else
            str.remove(0,1);

        if(state.loadfile(str.toStdString().c_str()) != 0)
        {
            hide();
            QMessageBox::critical(nullptr,tr("Error loading gamemode %1").arg(m_gamemode.Name),tr("Lua Error while opening module file %1.\nLua Error: %2").arg(str).arg(QString::fromStdString(state.tostdstring(1))));
            CloseToStartupWindow();
            return;
        }
        if(state.pcall() != 0)
        {
            hide();
            QMessageBox::critical(nullptr,tr("Error loading gamemode %1").arg(m_gamemode.Name),tr("Lua Error while loading module file %1.\nLua Error: %2").arg(str).arg(QString::fromStdString(state.tostdstring(1))));
            CloseToStartupWindow();
            return;
        }
    }

    subRegisterLuaFunctions();
}

void GameWindow::CloseToStartupWindow()
{
    if(m_startupWindow)
        m_startupWindow->SetShowOnClose(true);
    close();
}

void GameWindow::CloseToDesktop()
{
    if(m_startupWindow)
        m_startupWindow->SetShowOnClose(false);
    close();
}

void GameWindow::resizeGL(int w, int h)
{
    if(state.getglobal("resize") != LUA_TFUNCTION)
    {
        state.pop(1);
        return;
    }
    state.pushnumber(w);
    state.pushnumber(h);
    if(state.pcall(2) != 0)
    {
        hide();
        QMessageBox::critical(nullptr,tr("Error running gamemode %1").arg(m_gamemode.Name),tr("Lua Error while resizing window.\nLua Error: %1").arg(QString::fromStdString(state.tostdstring(1))));
        CloseToStartupWindow();
        return;
    }
}

void GameWindow::paintGL()
{
    if(state.getglobal("frame") != LUA_TFUNCTION)
    {
        state.pop(1);
        return;
    }
    if(state.pcall() != 0)
    {
        hide();
        QMessageBox::critical(nullptr,tr("Error running gamemode %1").arg(m_gamemode.Name),tr("Lua Error while rendering window.\nLua Error: %1").arg(QString::fromStdString(state.tostdstring(1))));
        CloseToStartupWindow();
        return;
    }
    this->update();
}

void GameWindow::paintUnderGL()
{
    if(state.getglobal("begin_frame") != LUA_TFUNCTION)
    {
        state.pop(1);
        return;
    }
    if(state.pcall() != 0)
    {
        hide();
        QMessageBox::critical(nullptr,tr("Error running gamemode %1").arg(m_gamemode.Name),tr("Lua Error while rendering window.\nLua Error: %1").arg(QString::fromStdString(state.tostdstring(1))));
        CloseToStartupWindow();
        return;
    }
}

void GameWindow::paintOverGL()
{
    if(state.getglobal("end_frame") != LUA_TFUNCTION)
    {
        state.pop(1);
        return;
    }
    if(state.pcall() != 0)
    {
        hide();
        QMessageBox::critical(nullptr,tr("Error running gamemode %1").arg(m_gamemode.Name),tr("Lua Error while rendering window.\nLua Error: %1").arg(QString::fromStdString(state.tostdstring(1))));
        CloseToStartupWindow();
        return;
    }
}

GameWindow::~GameWindow()
{
}
