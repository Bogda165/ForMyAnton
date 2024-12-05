#include <QApplication>
#include "ChatApp.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QString argument;
    if (argc > 1) {
        argument = argv[1]; // Get the port from the command-line arguments
    } else {
        qDebug() << "No port provided. Using default port.";
        argument = "default_port"; // Set a default port if none is provided
    }
    ChatApp chatApp("/Users/user/CLionProjects/PKS/Pks_project/cmake-build-debug/bin/writeFifo" + argument, "/Users/user/CLionProjects/PKS/Pks_project/cmake-build-debug/bin/readFifo" + argument);
    chatApp.setWindowTitle("Simple Chat");
    chatApp.resize(400, 600);
    chatApp.show();

    return QApplication::exec();
}
