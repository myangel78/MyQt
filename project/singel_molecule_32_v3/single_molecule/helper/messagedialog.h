#ifndef _ECOLI_MESSAGEBOXHELPER_H
#define _ECOLI_MESSAGEBOXHELPER_H

#include <qstring.h>

class MessageDialogHelper
{
public:
    static void initErrorDialog();

public:
    static QString showInputBox(const QString& info, bool blok);
    static QString showInputPasswordBox(const QString& info, bool blok);
    static void showMessageBoxInfo(const QString& info);
    static void showMessageBoxWarn(const QString& info);
    static void showMessageBoxError(const QString& info);
    static int showMessageBoxQuesion(const QString& info);

public:
};

#endif // _ECOLI_MESSAGEBOXHELPER_H
