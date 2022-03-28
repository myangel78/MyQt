#pragma once

#include <QDialog>
#include "Global.h"

class QLineEdit;
class QComboBox;
class QSpinBox;
class QLabel;

using namespace  Cyclone;

class InputNewRecordDlg : public QDialog
{
public:
    InputNewRecordDlg(DLG_TYPE_ENUM type,QWidget *parent = nullptr);

    QLabel *m_pRegIdLab ;
    QLabel *m_pRegNameLab;
    QLabel *m_pRegAddrLab;
    QLabel *m_pRegSizeLab;
    QLabel *m_pRegDefVueLab;
    QLabel *m_pRegVueLab;
    QLabel *m_pRegFielTabNameLab;

    QSpinBox *m_pRegIdSpbox;
    QLineEdit *m_pRegNameEdit;
    QLineEdit *m_pRegAddrEdit;
    QComboBox *m_pRegSizeComb;
    QLineEdit *m_pRegDefVueEdit;
    QLineEdit *m_pRegVueEdit;
    QLineEdit *m_pRegFielTabNameEdit;

    QPushButton *m_pConfirmBtn;
    QPushButton *m_pCancelBtn;

    void SetId(const int &id);
    void GetInRegIdItemStu(RegIdItemSimp &item);
    void LoaddingData(const RegIdItemSimp &item);

private slots:
    void ConfirmBtnSlot(void);

private:
    void InitCtr(void);
    bool CheckAllValid(void);
};

