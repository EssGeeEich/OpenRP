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
    std::string DataPath() const;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void paintUnderGL() override;
    void paintOverGL() override;
    
    void focusInEvent(QFocusEvent *) override;
    void focusOutEvent(QFocusEvent *) override;
    void hideEvent(QHideEvent *) override;
    void exposeEvent(QExposeEvent *) override;
    
    void keyPressEvent(QKeyEvent *) override;
    void keyReleaseEvent(QKeyEvent *) override;
    
    void mouseMoveEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *) override;

    void printContextInformations();
    void registerLuaFunctions();

    void CloseToStartupWindow();
    void CloseToDesktop();
    
private:
    bool preCallLuaFunction(char const*);
    bool callLuaFunction(char const*, int =0);
    void subRegisterLuaFunctions();
    void glToLua();
    Lua::State state;

    GameMode m_gamemode;
    StartupWindow* m_startupWindow;
    LuaApi::Storage m_storage;
    QString m_basePath;
};

#endif // GAMEWINDOW_H
