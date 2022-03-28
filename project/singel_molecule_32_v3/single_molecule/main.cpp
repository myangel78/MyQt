#include "view/mainwindow.h"
#include <QApplication>
#include "Log.h"
#include"helper/appconfig.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AppConfig::initGlobalFolder();

    //init log print
    ecoli::CEcoliLog::Init();

    MainWindow w;
    w.show();
    return a.exec();
}
