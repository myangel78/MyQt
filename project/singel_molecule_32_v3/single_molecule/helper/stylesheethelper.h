#ifndef _ECOLI_STYLESHEETHELPER_H
#define _ECOLI_STYLESHEETHELPER_H

#include <QtCore>
#include <QtGui>


class StyleSheetHelper: public QObject
{
public:
    static void loadStyleSheet(const QString style, QWidget* pThis = NULL)
    {
//        QFile file(style);
//        if (file.open(QFile::ReadOnly))
//        {
//            QString qss = QLatin1String(file.readAll());
//            if(pThis == NULL)
//                qApp->setStyleSheet(file.readAll());
//            else
//                pThis->setStyleSheet(file.readAll());
//            QString PaletteColor = qss.mid(20, 7);
//            if(pThis == NULL)
//                qApp->setPalette(QPalette(QColor(PaletteColor)));
//            else
//                pThis->setPalette(QPalette(QColor(PaletteColor)));
//            file.close();
//        }
        QFile styleSheet(style);
        if (!styleSheet.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qWarning("Can't open the style sheet file.");
            return;
        }
        if(pThis != NULL)
            pThis->setStyleSheet(styleSheet.readAll());
        else
            qApp->setStyleSheet(styleSheet.readAll());

        styleSheet.close();
    }
};

#endif // _ECOLI_ANIMATIONBUTTON_H
