#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QOpenGLWindow>
#include "lua_ogl_link.h"
#include "state.h"
#include "gamemode.h"

class StartupWindow;

class GameWindow : public QOpenGLWindow, public GL_t
{
    Q_OBJECT

public:
    explicit GameWindow(QWindow* parent, GameMode gamemode, StartupWindow* startupWindow);
    ~GameWindow();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void paintUnderGL() override;
    void paintOverGL() override;

    void printContextInformations();
    void registerLuaFunctions();

    void CloseToStartupWindow();
    void CloseToDesktop();
private:
    void subRegisterLuaFunctions();
    void glToLua();
    Lua::State state;

    GameMode m_gamemode;
    StartupWindow* m_startupWindow;
    LuaApi::Storage m_storage;
};

#endif // GAMEWINDOW_H
