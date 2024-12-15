#include <QApplication>

#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow window;
    window.resize(1200, 700);
    window.show();
    return QApplication::exec();
}
