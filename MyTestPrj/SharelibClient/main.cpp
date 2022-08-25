#include <QCoreApplication>
#include <QDebug>
#include "ShareInherit.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    // 测试库
    Sharelib lib;
    qDebug() << "Sharelib.add"<<lib.add(2, 3);
    qDebug() << "Sharelib.subtraction"<<lib.subtraction(15, 5);
    qDebug() << "subtract"<< subtract(5, 2);

    qDebug()<<"-----------------------";


    Sharelib *pSharelib = new ShareInherit();
    qDebug() << "pSharelib->add"<< pSharelib->add(100, 100);
    qDebug() << "pSharelib->subtraction"<< pSharelib->subtraction(100, 50);
    qDebug() << "pSharelib->multiply"<< (dynamic_cast<ShareInherit *>(pSharelib))->multiply(100, 100);


    return a.exec();
}
