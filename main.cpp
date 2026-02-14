#include <QApplication>
#include <QIcon>
#include <QStyleFactory>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("QtCode");
    app.setWindowIcon(QIcon(":/icon.svg"));

    // Use Fusion style for consistent theming
    app.setStyle(QStyleFactory::create("Fusion"));

    MainWindow window;
    window.show();

    // Open file if passed as argument
    if (argc > 1) {
        window.openFile(argv[1]);
    }

    return app.exec();
}
