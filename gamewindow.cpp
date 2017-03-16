#include "gamewindow.h"
#include <QCoreApplication>
#include <QMessageBox>
#include <QDir>
#include <QKeyEvent>
#include <QMouseEvent>
#include <cmath>
#include "startupwindow.h"

GameWindow::GameWindow(QWindow* parent, GameMode gamemode, StartupWindow* startupWindow) :
    QOpenGLWindow(QOpenGLWindow::NoPartialUpdate, parent),
    state(Lua::State::create()),
    m_gamemode(gamemode),
    m_startupWindow(startupWindow),
    m_basePath(QDir::currentPath())
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

void GameWindow::FatalError(QString const& title, QString const& description)
{
    hide();
    QMessageBox::critical(nullptr,title,description);
    CloseToStartupWindow();
}

void GameWindow::initializeGL()
{
    initializeOpenGLFunctions();
    printContextInformations();
    
    if(!registerLuaFunctions())
        return;

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

std::string GameWindow::DataPath() const
{
     return m_basePath.toStdString() + "/gamemode/" + m_gamemode.SubFolder.toStdString() + "/data/";
}

void GameWindow::SetRequireCPath(std::string const& cpath)
{
    state.getglobal("package");
    state.pushstdstring(cpath);
    state.setfield(-2, "cpath");
    state.pop(1);
}

void GameWindow::SetRequirePath(std::string const& path)
{
    state.getglobal("package");
    state.pushstdstring(path);
    state.setfield(-2, "path");
    state.pop(1);
}

bool GameWindow::registerLuaFunctions()
{
    {
        const luaL_Reg* lib;
        for(lib = loadedlibs; lib->func; lib++) {
            state.requiref(lib->name,lib->func,1);
            state.pop(1);
        }
    }
    SetRequireCPath(std::string());
    SetRequirePath(std::string());
    try {
        m_link.Init(this,this,state);
    } catch(std::exception& e) {
        FatalError(tr("Loading Error"),e.what());
        return false;
    }

    std::string ApiPath = m_basePath.toStdString() + "/api/";
    std::string GmPath = m_basePath.toStdString() + "/gamemode/" + m_gamemode.SubFolder.toStdString() + "/";
    
    std::string ApiRequirements =
            ApiPath + "?.lua;" +
            ApiPath + "modules/?.lua;" +
            ApiPath + "lua/?.lua";
    
    std::string GamemodeRequirements =
            ApiRequirements + ";" +
            GmPath + "?.lua;" +
            GmPath + "modules/?.lua;" +
            GmPath + "lua/?.lua";
    
    for(auto it = m_gamemode.Modules.begin(); it != m_gamemode.Modules.end(); ++it)
    {
        QString str = *it;
        if(str.at(0) != '/')
        {
            str = m_basePath + "/gamemode/" + m_gamemode.SubFolder + "/" + *it;
            SetRequirePath(GamemodeRequirements);
        }
        else
        {
            str.remove(0,1);
            str = m_basePath + "/" + str;
            SetRequirePath(ApiRequirements);
        }

        if(state.loadfile(str.toStdString().c_str()) != 0)
        {
            FatalError(tr("Error loading gamemode %1").arg(m_gamemode.Name),tr("Lua Error while opening module file %1.\nLua Error: %2").arg(str).arg(QString::fromStdString(state.tostdstring(1))));
            return false;
        }
        if(state.pcall() != 0)
        {
            FatalError(tr("Error loading gamemode %1").arg(m_gamemode.Name),tr("Lua Error while loading module file %1.\nLua Error: %2").arg(str).arg(QString::fromStdString(state.tostdstring(1))));
            return false;
        }
    }
    
    SetRequirePath(GamemodeRequirements);
    return true;
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

bool GameWindow::preCallLuaFunction(const char* name)
{
    if(state.getglobal(name) != LUA_TFUNCTION)
    {
        state.pop(1);
        return false;
    }
    return true;
}

bool GameWindow::callLuaFunction(const char* name, int args)
{
    if(state.pcall(args) != 0)
    {
        hide();
        QMessageBox::critical(nullptr,tr("Error running gamemode %1").arg(m_gamemode.Name),
                              tr("Lua Error while calling fundamental function: %1.\nLua Error: %2").arg(QString(name)).arg(QString::fromStdString(state.tostdstring(1))));
        CloseToStartupWindow();
        return false;
    }
    return true;
}

void GameWindow::resizeGL(int w, int h)
{
    QOpenGLWindow::resizeGL(w,h);
    
    if(preCallLuaFunction("resize"))
    {
        state.pushnumber(w);
        state.pushnumber(h);
        callLuaFunction("resize",2);
    }
}

void GameWindow::paintGL()
{
    QOpenGLWindow::paintGL();
    
    if(preCallLuaFunction("frame") &&
        callLuaFunction("frame"))
    {
        this->update();
    }
}

void GameWindow::paintUnderGL()
{
    QOpenGLWindow::paintUnderGL();
    
    if(preCallLuaFunction("begin_frame"))
        callLuaFunction("begin_frame");
}

void GameWindow::paintOverGL()
{
    QOpenGLWindow::paintOverGL();
    
    if(preCallLuaFunction("end_frame"))
        callLuaFunction("end_frame");
}

void GameWindow::focusInEvent(QFocusEvent* e)
{
    QOpenGLWindow::focusInEvent(e);
    
    if(preCallLuaFunction("focus"))
    {
        state.pushboolean(true);
        if(callLuaFunction("focus",1))
            e->accept();
    }
}

void GameWindow::focusOutEvent(QFocusEvent* e)
{
    QOpenGLWindow::focusOutEvent(e);
    
    if(preCallLuaFunction("focus"))
    {
        state.pushboolean(false);
        if(callLuaFunction("focus",1))
            e->accept();
    }
}

void GameWindow::hideEvent(QHideEvent* e)
{
    QOpenGLWindow::hideEvent(e);
    
    if(preCallLuaFunction("show"))
    {
        state.pushboolean(false);
        if(callLuaFunction("show",1))
            e->accept();
    }
}

void GameWindow::exposeEvent(QExposeEvent* e)
{
    QOpenGLWindow::exposeEvent(e);
    
    if(preCallLuaFunction("show"))
    {
        state.pushboolean(true);
        if(callLuaFunction("show",1))
            e->accept();
    }
}

void GameWindow::keyPressEvent(QKeyEvent* e)
{
    QOpenGLWindow::keyPressEvent(e);
    
    if(preCallLuaFunction("keydown"))
    {
        state.pushinteger(e->key());
        state.pushinteger(e->nativeScanCode());
        state.pushboolean(e->isAutoRepeat());
        state.pushboolean(e->modifiers());
        if(callLuaFunction("keydown",4))
            e->accept();
    }
}
void GameWindow::keyReleaseEvent(QKeyEvent* e)
{
    QOpenGLWindow::keyReleaseEvent(e);
    
    if(preCallLuaFunction("keyup"))
    {
        state.pushinteger(e->key());
        state.pushinteger(e->nativeScanCode());
        state.pushboolean(e->isAutoRepeat());
        state.pushboolean(e->modifiers());
        if(callLuaFunction("keyup",4))
            e->accept();
    }
}
void GameWindow::mouseMoveEvent(QMouseEvent* e)
{
    QOpenGLWindow::mouseMoveEvent(e);
    
    if(preCallLuaFunction("mousemove"))
    {
        state.pushinteger(e->x());
        state.pushinteger(e->y());
        state.pushinteger(e->buttons());
        if(callLuaFunction("mousemove",3))
            e->accept();
    }
}
void GameWindow::mousePressEvent(QMouseEvent* e)
{
    QOpenGLWindow::mousePressEvent(e);
    
    if(preCallLuaFunction("mousedown"))
    {
        state.pushinteger(e->button());
        state.pushinteger(e->x());
        state.pushinteger(e->y());
        state.pushinteger(e->buttons());
        if(callLuaFunction("mousedown",4))
            e->accept();
    }
}
void GameWindow::mouseReleaseEvent(QMouseEvent* e)
{
    QOpenGLWindow::mouseReleaseEvent(e);
    
    if(preCallLuaFunction("mouseup"))
    {
        state.pushinteger(e->button());
        state.pushinteger(e->x());
        state.pushinteger(e->y());
        state.pushinteger(e->buttons());
        if(callLuaFunction("mouseup",4))
            e->accept();
    }
}
static double const inv120 = 1.0 / 120.0;
void GameWindow::wheelEvent(QWheelEvent* e)
{
    QOpenGLWindow::wheelEvent(e);
    
    QPoint numPixels = e->pixelDelta();
    QPoint numDegrees = e->angleDelta();
    if(numPixels.isNull() && numDegrees.isNull())
        return;
    
    std::int32_t x = 0;
    std::int32_t y = 0;
    
    if(numPixels.isNull())
    {
        double fxDelta = static_cast<double>(numDegrees.x()) * inv120;
        double fyDelta = static_cast<double>(numDegrees.y()) * inv120;
        x = std::ceil(fxDelta);
        y = std::ceil(fyDelta);
    }
    else
    {
        x = numPixels.x();
        y = numPixels.y();
    }
    if(x == 0 &&
        y == 0)
        return;
    
    if(preCallLuaFunction("wheel"))
    {
        state.pushnumber(x);
        state.pushnumber(y);
        state.pushnumber(e->x());
        state.pushnumber(e->y());
        state.pushnumber(e->buttons());
        if(callLuaFunction("wheel",5))
            e->accept();
    }
}

GameWindow::~GameWindow()
{
}
