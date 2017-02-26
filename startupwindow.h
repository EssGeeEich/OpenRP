#ifndef STARTUPWINDOW_H
#define STARTUPWINDOW_H

#include <QMainWindow>

namespace Ui {
class StartupWindow;
}

class GameWindow;

class StartupWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StartupWindow(QWidget *parent = 0);
    ~StartupWindow();
    void SetShowOnClose(bool);
    bool ShowOnClose() const;

public slots:
    void StartGame();

private:
    Ui::StartupWindow *ui;
    GameWindow* m_currentGame;
    bool m_showOnClose;
};

#endif // STARTUPWINDOW_H
