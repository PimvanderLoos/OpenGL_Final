#include "MainWindow/mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <ctime>

using namespace std;

int main(int argc, char *argv[])
{
//    srand(std::time(nullptr));
    QApplication app(argc, argv);

    // Request OpenGL 3.3 Core
    QSurfaceFormat glFormat;
    glFormat.setProfile(QSurfaceFormat::CoreProfile);
    glFormat.setVersion(3, 3);
    glFormat.setOption(QSurfaceFormat::DebugContext);

//    glFormat.setSwapInterval(0); // No V-Sync.

    // Some platforms need to explicitly set the depth buffer size (24 bits)
    glFormat.setDepthBufferSize(24);

    QSurfaceFormat::setDefaultFormat(glFormat);

    MainWindow w;
    w.show();

    return app.exec();
}
