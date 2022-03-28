#include "SampleRateDialog.h"
#include <qlayout.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <Log.h>

#include "MainWindow.h"


SampleRateDialog::SampleRateDialog(QWidget *parent)
    : QDialog(parent)
{
    InitCtrl();

    m_pMainWindow = dynamic_cast<MainWindow*>(parent);
    OnClickUpdateSample();
}

//SampleRateDialog::~SampleRateDialog()
//{
//    delete m_pgrpUpdateSample;
//    delete m_plabUpdateSample;
//    delete m_pbtnUpdateSample;
//    delete m_pgrpSetSample;
//    delete m_plabSampleRate;
//    delete m_pcmbSampleRate;
//    delete m_plabOverload;
//    delete m_pcmbOverload;
//}

bool SampleRateDialog::InitCtrl(void)
{
    m_pgrpUpdateSample = new QGroupBox("Update", this);

    m_plabUpdateSample = new QLabel("Current Sample: 5KX4", m_pgrpUpdateSample);
    m_pbtnUpdateSample = new QPushButton("Update", m_pgrpUpdateSample);

    QHBoxLayout* horlayout1 = new QHBoxLayout();
    horlayout1->addWidget(m_plabUpdateSample);
    horlayout1->addWidget(m_pbtnUpdateSample);

    m_pgrpUpdateSample->setLayout(horlayout1);


    m_pgrpSetSample = new QGroupBox("Sample Rate", this);
    m_pgrpSetSample->setCheckable(true);
    m_pgrpSetSample->setChecked(false);

    m_plabSampleRate = new QLabel("Sample Frequency", m_pgrpSetSample);
    m_pcmbSampleRate = new QComboBox(m_pgrpSetSample);
    m_pcmbSampleRate->addItem("5K", SAMPLE_RATE_5K);
    m_pcmbSampleRate->addItem("20K", SAMPLE_RATE_20K);

    m_plabOverload = new QLabel("Over Multiple", m_pgrpSetSample);
    m_pcmbOverload = new QComboBox(m_pgrpSetSample);
    m_pcmbOverload->addItem("X1", EOM_1X);
    m_pcmbOverload->addItem("X2", EOM_2X);
    m_pcmbOverload->addItem("X4", EOM_4X);
    m_pcmbOverload->addItem("X8", EOM_8X);
    m_pcmbOverload->setCurrentIndex(2);

    QGridLayout* gdlayout1 = new QGridLayout();
    gdlayout1->addWidget(m_plabSampleRate, 0, 0);
    gdlayout1->addWidget(m_pcmbSampleRate, 0, 1);
    gdlayout1->addWidget(m_plabOverload, 1, 0);
    gdlayout1->addWidget(m_pcmbOverload, 1, 1);

    m_pgrpSetSample->setLayout(gdlayout1);


    QVBoxLayout* verlayout9 = new QVBoxLayout();
    verlayout9->addWidget(m_pgrpUpdateSample);
    verlayout9->addWidget(m_pgrpSetSample);

    setLayout(verlayout9);


    connect(m_pbtnUpdateSample, &QPushButton::clicked, this, &SampleRateDialog::OnClickUpdateSample);
    connect(m_pgrpSetSample, &QGroupBox::clicked, this, &SampleRateDialog::OnCheckSampleRate);
    connect(m_pcmbSampleRate, SIGNAL(currentIndexChanged(int)), this, SLOT(OnChangeSampleFrequency(int)));
    connect(m_pcmbOverload, SIGNAL(currentIndexChanged(int)), this, SLOT(OnChangeOverMultiple(int)));

    return false;
}

void SampleRateDialog::OnClickUpdateSample(void)
{
    if (m_pMainWindow == nullptr)
    {
        LOGE("mainpoint not setted!!!");
        return;
    }
    const unsigned short sampleAddr = 0x04;
    const unsigned short overlaodAddr = 0x01;
    unsigned int sampleValue = 0;
    unsigned int overloadValue = 0;
    bool sampleRet = false;
    bool overlaodRet = false;
    QString samplestr = "__";
    QString overloadstr = "__";

    m_pMainWindow->FPGARegisterReadBack(sampleAddr, sampleValue, sampleRet);
    if (sampleRet)
    {
        switch (sampleValue)
        {
        case MainWindow::ESR_SAMPLE_5K:
        {
            samplestr = " 5K";
            if (m_pcmbSampleRate->currentIndex() != 0)
            {
                m_pcmbSampleRate->setCurrentIndex(1);
                m_pMainWindow->SetSampleRateChanged(SAMPLE_RATE_5K);
            }
            break;
        }
        case MainWindow::ESR_SAMPLE_20K:
        {
            samplestr = "20K";
            if (m_pcmbSampleRate->currentIndex() != 1)
            {
                m_pcmbSampleRate->setCurrentIndex(1);
                m_pMainWindow->SetSampleRateChanged(SAMPLE_RATE_20K);
            }
            break;
        }
        default:
            break;
        }
    }
    else
    {
        //QMessageBox::warning(this, "Sample Rate", "Get Sample Rate FAILED!!!");
        LOGE("Get Sample Rate FAILED!!!");
        //return;
    }

    m_pMainWindow->FPGARegisterReadBack(overlaodAddr, overloadValue, overlaodRet);
    if (overlaodRet)
    {
        switch (overloadValue)
        {
        case MainWindow::EOR_OVERLOAD_1X_DEFAULT:
        case MainWindow::EOR_OVERLOAD_1X:
        {
            overloadstr = "X1";
            if (m_pcmbOverload->currentIndex() != 0)
            {
                m_pcmbOverload->setCurrentIndex(0);
            }
            break;
        }
        case MainWindow::EOR_OVERLOAD_2X:
        {
            overloadstr = "X2";
            if (m_pcmbOverload->currentIndex() != 1)
            {
                m_pcmbOverload->setCurrentIndex(1);
            }
            break;
        }
        case MainWindow::EOR_OVERLOAD_4X:
        {
            overloadstr = "X4";
            if (m_pcmbOverload->currentIndex() != 2)
            {
                m_pcmbOverload->setCurrentIndex(2);
            }
            break;
        }
        case MainWindow::EOR_OVERLOAD_8X:
        {
            overloadstr = "X8";
            if (m_pcmbOverload->currentIndex() != 3)
            {
                m_pcmbOverload->setCurrentIndex(3);
            }
            break;
        }
        default:
            break;
        }
    }
    else
    {
        //QMessageBox::warning(this, "Overload Rate", "Get Overload Rate FAILED!!!");
        LOGE("Get Over Multiple FAILED!!!");
        //return;
    }
    m_plabUpdateSample->setText(QString("Current Sample:") + samplestr + overloadstr);
}

void SampleRateDialog::OnCheckSampleRate(bool chk)
{
    if (chk)
    {
        bool ok = false;
        QString text = QInputDialog::getText(this, QString::fromLocal8Bit("Confirm Password"), QString::fromLocal8Bit("Please Enter the Password."), QLineEdit::Password, 0, &ok);
        if (ok)
        {
            if (QString::compare(text, PROGRAME_PASSWORD) == 0)
            {
                return;
            }
            else
            {
                QMessageBox::warning(this, "Warning ", tr("Wrong Password!"), QMessageBox::Ok);
            }
        }

        m_pgrpSetSample->setChecked(false);
    }
}

void SampleRateDialog::OnChangeSampleFrequency(int val)
{
    int rate = m_pcmbSampleRate->currentData().toInt();
    m_pMainWindow->SetSampleRateChanged(rate);
    OnClickUpdateSample();
}

void SampleRateDialog::OnChangeOverMultiple(int val)
{
    int rate = m_pcmbOverload->currentData().toInt();
    m_pMainWindow->SetOverMultipleChanged(rate);
    OnClickUpdateSample();
}
