#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Notepad");

    MainWindow window;
    window.show();

    // Open file if passed as argument
    if (argc > 1) {
        window.openFile(argv[1]);
    }

    return app.exec();
}
