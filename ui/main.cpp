#include <QApplication>

#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setDesktopFileName("com.github.draqzziq.riscvsimulator");
    MainWindow window;
    window.resize(1200, 700);
    window.show();
    return QApplication::exec();
}
