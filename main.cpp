#include "MainWindow.h"

#include <QApplication>
#include"BugLog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("中国象棋");
    w.show();
    auto bugLog = new BugLog();

    return a.exec();
}
