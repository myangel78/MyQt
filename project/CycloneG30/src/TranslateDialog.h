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
    QPushButton* m_pbtnBinaryTranslatToRawData;
    QTextEdit* m_ptxtTextContent;

private:
    bool InitCtrl(void);
    bool InverseTransformToRawData(const std::vector<float> &vctSource,std::vector<float> &vctDest,const size_t &length);


private slots:
    void OnClickBinaryTranslate2Text(void);
    void OnClickOpenTextFile(void);
    void OnClickBinaryTranslateToRawData(void);

signals:

};

