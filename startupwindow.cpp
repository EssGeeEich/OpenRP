#include "startupwindow.h"
#include "ui_startupwindow.h"
#include "gamewindow.h"
#include "gamemode.h"
#include <QSurfaceFormat>

static QList<GameMode> const g_gamemodes = {
    { "Roleplay", "rp", QStringList() << "main.lua" }
};

StartupWindow::StartupWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StartupWindow),
    m_currentGame(nullptr),
    m_showOnClose(false)
{
    ui->setupUi(this);
    connect(ui->playNowButton, &QPushButton::clicked, this, &StartupWindow::StartGame);

    int i = 0;
    for(auto it = g_gamemodes.begin(); it != g_gamemodes.end(); ++it)
    {
        ui->gamemodeList->addItem(it->Name, i);
    }

    if(ui->gamemodeList->count() > 0)
    {
        ui->gamemodeList->setCurrentIndex(0);
        ui->playNowButton->setEnabled(true);
        if(ui->gamemodeList->count() > 1)
            ui->gamemodeList->setEnabled(true);
    }
}

void StartupWindow::StartGame()
{
    if(m_currentGame ||
            ui->gamemodeList->currentIndex() < 0 ||
            ui->gamemodeList->currentIndex() >= ui->gamemodeList->count())
        return;

    bool ok = false;
    int gamemodeIndex = ui->gamemodeList->currentData().toInt(&ok);
    if(!ok || gamemodeIndex >= g_gamemodes.size())
        return;

    GameMode gamemode = g_gamemodes.at(gamemodeIndex);
    if(gamemode.Modules.isEmpty())
        return;
    gamemode.Modules.insert(gamemode.Modules.begin(),"/api/openrp.lua");

    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4,4);

    m_currentGame = new GameWindow(nullptr, gamemode, this);

    connect(m_currentGame, &GameWindow::destroyed, [&]()->void{
        if(this->ShowOnClose())
        {
            m_currentGame = nullptr;
            this->show();
        }
        else
            this->close();
    });
    hide();

    m_currentGame->setFormat(format);
    m_currentGame->resize(800, 600);
    m_currentGame->show();
}

void StartupWindow::SetShowOnClose(bool s)
{
    m_showOnClose = s;
}
bool StartupWindow::ShowOnClose() const
{
    return m_showOnClose;
}

StartupWindow::~StartupWindow()
{
    delete ui;
}
