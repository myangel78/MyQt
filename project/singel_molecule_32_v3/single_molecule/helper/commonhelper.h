#ifndef COMMONHELPER_H
#define COMMONHELPER_H

#include <sstream>
#include <vector>
#include <regex>
#include <QtCore>
#include <QtGui>
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif
#include "helper/appconfig.h"
#include "logger/Log.h"

class CommonHelper
{
public:

    static void setStyle(const QString &qssFile)
    {
        QFile file(qssFile);
        if (file.open(QFile::ReadOnly))
        {
            QString qss = QLatin1String(file.readAll());
            qApp->setStyleSheet(qss);
            QString PaletteColor = qss.mid(20, 7);
            qApp->setPalette(QPalette(QColor(PaletteColor)));
            file.close();
        }
    }

    static void setChinese()
    {
        QTranslator *translator = new QTranslator(qApp);
        translator->load(":/image/qt_zh_CN.qm");
        qApp->installTranslator(translator);
    }


    static bool isNumeric(const char* pValue)
    {
        return (pValue != NULL) && std::regex_match(pValue, std::regex("^(\\-|\\+)?[0-9]*(\\.[0-9]+)?"));
    }

    static void moveFormToCenter(QWidget *frm)
    {
        int frmX = frm->width();
        int frmY = frm->height();

        QDesktopWidget dwt;

        int deskWidth = frm->parentWidget()!=0?frm->parentWidget()->width():dwt.availableGeometry().width();
        int deskHeight = frm->parentWidget()!=0?frm->parentWidget()->height():dwt.availableGeometry().height();

        QPoint movePoint(deskWidth / 2 - frmX / 2, deskHeight / 2 - frmY / 2);
        frm->move(movePoint);
    }

    static void adjustComboBoxItemHight(QComboBox* pCombo)
    {
        if(pCombo != NULL)
        {
            QListView *view = new QListView(pCombo);
            view->setStyleSheet("QListView::item{height: 30px}");

            pCombo->setView(view);
        }
    }

    static void adjustAllComboBox(QWidget* ui)
    {
        if(ui != NULL)
        {
            QList<QComboBox *> combos = ui->findChildren<QComboBox *>();
            foreach (QComboBox * c, combos)
            {
                CommonHelper::adjustComboBoxItemHight(c);
            }
        }
    }

    static void configSysDateTime(const std::string& strDateTime)
    {
        LOGI("current config datetime: {}", strDateTime.c_str());
#ifdef linux
        std::string cmdDate = "date -s \"" + strDateTime + "\"";
        int ret = system(cmdDate.c_str());
        cmdDate = "hwclock --systohc --utc";
        ret = system(cmdDate.c_str());
        (void) ret;
#endif
        AppConfig::writeConfigDateTime(QString::fromStdString(strDateTime));
    }


    template <class T>
    static std::string listToString(std::vector<T> l)
    {
        std::stringstream ss;
        for(typename std::vector<T>::iterator it = l.begin(); it!=l.end(); ++it)
        {
            ss << *it;
            if(std::distance(it,l.end())>1)
                ss << ", ";
        }
        return ss.str();
    }

    static bool isValidPath(const std::string& /*path*/)
    {
        //return access(path.c_str(), F_OK) == 0;
    }
};

#endif // COMMONHELPER_H
