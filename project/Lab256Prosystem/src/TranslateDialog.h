#pragma once

#include <QDialog>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtextedit.h>


class TranslateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TranslateDialog(QWidget *parent = nullptr);
    //virtual ~TranslateDialog();

private:
    QLabel* m_plabFileName;
    QPushButton* m_pbtnBinaryTranslat2Text;
    QPushButton* m_pbtnOpenTextFile;
    QTextEdit* m_ptxtTextContent;

private:
    bool InitCtrl(void);


private slots:
    void OnClickBinaryTranslate2Text(void);
    void OnClickOpenTextFile(void);

signals:

};

