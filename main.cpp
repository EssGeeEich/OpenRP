#include "startupwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    QCoreApplication::setApplicationVersion(
        (std::to_string(VERSION_MAJOR) + "." +
         std::to_string(VERSION_MINOR) + "." +
         std::to_string(VERSION_PATCH)).c_str()
    );

    QApplication a(argc, argv);
    StartupWindow w;
    w.show();

    return a.exec();
}
