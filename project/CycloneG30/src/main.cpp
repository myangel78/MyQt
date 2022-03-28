#include "MainWindow.h"

#include <QApplication>
#include"Log.h"

#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"user32.lib")

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont font = a.font();
    font.setFamily("Microsoft YaHei");//微软雅黑字体
    a.setFont(font);
    MainWindow w;
    w.show();
    return a.exec();
}

