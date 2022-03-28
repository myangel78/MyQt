#include "messagedialog.h"
#include "view/confirmmessagedialog.h"
#include "view/inputmessagedialog.h"
#include "messagedialogdefine.h"

QString MessageDialogHelper::showInputBox(const QString& info, bool blok)
{
    InputMessageDialog input;

    input.setMessage(info);
    blok = input.exec();

    return input.getValue();
}


QString MessageDialogHelper::showInputPasswordBox(const QString& info, bool blok)
{
    InputMessageDialog input;

    input.setPasswordMessage(info);
    blok = input.exec();

    return input.getValue();
}


void MessageDialogHelper::showMessageBoxInfo(const QString& info)
{
    ConfirmMessageDialog msg;

    msg.setMessage(info, MSG_DLG_INFO);
    msg.exec();
}

void MessageDialogHelper::showMessageBoxWarn(const QString& info)
{
    ConfirmMessageDialog msg;

    msg.setMessage(info, MSG_DLG_WARNING);
    msg.exec();
}

ConfirmMessageDialog* g_pErrMsgDlg = NULL;
void MessageDialogHelper::initErrorDialog()
{
    g_pErrMsgDlg = new ConfirmMessageDialog();
}

void MessageDialogHelper::showMessageBoxError(const QString& info)
{    
    if(g_pErrMsgDlg == NULL ) initErrorDialog();

    g_pErrMsgDlg->setMessage(info, MSG_DLG_ERROR);
    if(!g_pErrMsgDlg->isVisible())
    {
        g_pErrMsgDlg->exec();
    }
}

int MessageDialogHelper::showMessageBoxQuesion(const QString& info)
{
    ConfirmMessageDialog msg;
    msg.setMessage(info, MSG_DLG_QUESTION);
    return msg.exec();
}

