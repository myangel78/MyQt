#include "CalibrationDialog.h"

#include <filesystem>
#include <qlayout.h>
#include <qlineedit.h>
#include <qfiledialog.h>
#include <qprogressdialog.h>
#include <qdatetime.h>
#include <qcoreapplication.h>
#include <qmessagebox.h>

#include <Log.h>



CalibrationDialog::CalibrationDialog(QAction* pactStart, QWidget *parent) 
    : QDialog(parent)
    , m_pactStart(pactStart)
{
    //Qt::WindowFlags flags = Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint;
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint /*| Qt::WindowMinimizeButtonHint*/ | Qt::WindowMaximizeButtonHint);
    setAttribute(Qt::WA_WState_WindowOpacitySet);

    m_vetCalib.resize(CHANNEL_NUM);
    m_vetPoints.resize(CHANNEL_NUM);
    resize(800, 400);

    std::string strcali = ConfigServer::GetUserPath() + "/etc/Conf/CalibrationConfig.ini";
    const char* pcali = strcali.c_str();
    bool ret = ConfigServer::SyncDefaultConfig(":/conf/conf/default", pcali);
    if (ret)
    {
        QFile file(pcali);
        if (file.exists())
        {
            file.setPermissions(file.permissions() | QFile::ReadOwner | QFile::WriteOwner);
        }
    }
    LoadConfig(pcali, &m_pSettingConfig);

    InitCtrl();
    InitTable();
}

CalibrationDialog::~CalibrationDialog()
{
    if (m_bChanged)
    {
        auto rsp = QMessageBox::question(this, tr("Save Calibration Changes"), tr("The param had been modified.\nDo you want to save your changes?"));
        if (rsp == QMessageBox::Yes)
        {
            SaveConfig(m_pSettingConfig);
        }
        m_bChanged = false;
    }
}

bool CalibrationDialog::InitCtrl(void)
{
    m_plabMaxVolt = new QLabel("MaxVolt:", this);
    m_pdspMaxVolt = new QDoubleSpinBox(this);
    m_pdspMaxVolt->setSuffix(" V");
    m_pdspMaxVolt->setValue(m_dMaxVolt);
    connect(m_pdspMaxVolt, &QDoubleSpinBox::textChanged, this, &CalibrationDialog::calibValueChangedSlot);

    m_plabStepVolt = new QLabel("StepVolt:", this);
    m_pdspStepVolt = new QDoubleSpinBox(this);
    m_pdspStepVolt->setSuffix(" V");
    m_pdspStepVolt->setValue(m_dStepVolt);
    connect(m_pdspStepVolt, &QDoubleSpinBox::textChanged, this, &CalibrationDialog::calibValueChangedSlot);

    m_plabCurVolt = new QLabel("CurrentVolt:", this);
    m_pdspCurVolt = new QDoubleSpinBox(this);
    m_pdspCurVolt->setSuffix(" V");
    m_pdspCurVolt->setValue(m_dCurVolt);
    connect(m_pdspCurVolt, &QDoubleSpinBox::textChanged, this, &CalibrationDialog::calibValueChangedSlot);


    m_pbtnAutoCalib = new QPushButton("AutoCalibration", this);
    connect(m_pbtnAutoCalib, &QPushButton::clicked, this, &CalibrationDialog::OnClickAutoCalibration);


    m_ptawCalib = new QTableWidget(CHANNEL_NUM, 5, this);


    m_pbtnLoad = new QPushButton("Load", this);
    connect(m_pbtnLoad, &QPushButton::clicked, this, &CalibrationDialog::OnClickLoadConfig);


    m_plabAllResistance = new QLabel("AllResistance", this);
    m_pdspAllResistance = new QDoubleSpinBox(this);
    m_pdspAllResistance->setSuffix(QStringLiteral(" M¦¸"));
    m_pdspAllResistance->setDecimals(2);
    m_pdspAllResistance->setSingleStep(10.0);
    m_pdspAllResistance->setMaximum(99999.9);
    m_pdspAllResistance->setValue(1000.0);

    m_pbtnAllResistance = new QPushButton("SetAllResistance", this);
    connect(m_pbtnAllResistance, &QPushButton::clicked, this, &CalibrationDialog::OnClickAllResistance);


    m_plabAllSlope = new QLabel("AllSlope", this);
    m_pdspAllSlope = new QDoubleSpinBox(this);
    m_pdspAllSlope->setDecimals(2);
    m_pdspAllSlope->setSingleStep(0.1);
    m_pdspAllSlope->setMaximum(99999.9);
    m_pdspAllSlope->setValue(2.0);

    m_pbtnAllSlope = new QPushButton("SetAllSlope", this);
    connect(m_pbtnAllSlope, &QPushButton::clicked, this, &CalibrationDialog::OnClickAllSlope);


    m_plabAllOffset = new QLabel("AllOffset", this);
    m_pdspAllOffset = new QDoubleSpinBox(this);
    m_pdspAllOffset->setDecimals(2);
    m_pdspAllOffset->setSingleStep(1.0);
    m_pdspAllOffset->setMaximum(99999.9);
    m_pdspAllOffset->setValue(0.0);

    m_pbtnAllOffset = new QPushButton("SetAllOffset", this);
    connect(m_pbtnAllOffset, &QPushButton::clicked, this, &CalibrationDialog::OnClickAllOffset);


    m_pbtnSave = new QPushButton("Save", this);
    connect(m_pbtnSave, &QPushButton::clicked, this, &CalibrationDialog::OnClickSaveChange);

    m_pbtnClear = new QPushButton("Clear", this);
    connect(m_pbtnClear, &QPushButton::clicked, this, &CalibrationDialog::OnClickClearChange);


    QHBoxLayout* horlayout1 = new QHBoxLayout();
    horlayout1->addWidget(m_plabMaxVolt);
    horlayout1->addWidget(m_pdspMaxVolt);
    horlayout1->addSpacing(20);
    horlayout1->addWidget(m_plabStepVolt);
    horlayout1->addWidget(m_pdspStepVolt);
    horlayout1->addSpacing(20);
    horlayout1->addWidget(m_plabCurVolt);
    horlayout1->addWidget(m_pdspCurVolt);
    horlayout1->addSpacing(50);
    horlayout1->addStretch(1);
    horlayout1->addWidget(m_pbtnAutoCalib);

    QHBoxLayout* horlayout2 = new QHBoxLayout();
    horlayout2->addWidget(m_pbtnLoad);
    horlayout2->addStretch(1);
    horlayout2->addWidget(m_pbtnSave);
    horlayout2->addWidget(m_pbtnClear);

    QHBoxLayout* horlayout3 = new QHBoxLayout();
    horlayout3->addWidget(m_plabAllResistance);
    horlayout3->addWidget(m_pdspAllResistance);
    horlayout3->addWidget(m_pbtnAllResistance);
    horlayout3->addSpacing(10);
    horlayout3->addWidget(m_plabAllSlope);
    horlayout3->addWidget(m_pdspAllSlope);
    horlayout3->addWidget(m_pbtnAllSlope);
    horlayout3->addSpacing(10);
    horlayout3->addWidget(m_plabAllOffset);
    horlayout3->addWidget(m_pdspAllOffset);
    horlayout3->addWidget(m_pbtnAllOffset);
    horlayout3->addStretch(1);

    QVBoxLayout* verlayout1 = new QVBoxLayout();
    verlayout1->addLayout(horlayout1);
    verlayout1->addWidget(m_ptawCalib);
    verlayout1->addLayout(horlayout3);
    verlayout1->addLayout(horlayout2);

    setLayout(verlayout1);

    return false;
}

bool CalibrationDialog::InitTable(void)
{
    QStringList strListHorHeader = QStringList() << "Channel" << "Resistence" << "Slope" << "Offset" << "Formula";
    m_ptawCalib->setColumnCount(strListHorHeader.size());
    m_ptawCalib->setRowCount(CHANNEL_NUM);
    m_ptawCalib->setHorizontalHeaderLabels(strListHorHeader);
    //m_ptawCalib->setVerticalHeaderLabels(QStringList());
    //m_ptawCalib->setShowGrid(true);
    m_ptawCalib->setColumnWidth(0, 60);
    m_ptawCalib->setColumnWidth(1, 100);
    m_ptawCalib->setColumnWidth(2, 150);
    m_ptawCalib->setColumnWidth(3, 150);
    m_ptawCalib->setColumnWidth(4, 260);

    //¨¦¨¨??¦Ì?¡Á¨¨¨º?¨¨??¨°???¨¹¨º?¨¨?double?¦Ì
    QDoubleValidator* resInputValidator = new QDoubleValidator(0, 100000, 3, this);
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        //Channel
        QTableWidgetItem* itemChan = new QTableWidgetItem(QString("CH%1").arg(i + 1));
        itemChan->setBackground(QBrush(QColor(Qt::white)));
        itemChan->setFlags(itemChan->flags() & (~Qt::ItemIsEditable));
        m_ptawCalib->setItem(i, 0, itemChan);
        //value
        const auto& calib = m_vetCalib[i];//m_mapCalib[i];
        auto resi = std::get<0>(calib);
        auto slop = std::get<1>(calib);
        auto offs = std::get<2>(calib);
        //Resistence
        QLineEdit* pResLineEdit = new QLineEdit(QString::number(resi, 'f', 3), this);
        pResLineEdit->setValidator(resInputValidator);
        //pResLineEdit->setPlaceholderText(QString("%1").arg(resi));
        //pResLineEdit->setText(QString("%1").arg(resi));
        m_ptawCalib->setCellWidget(i, 1, pResLineEdit);
        connect(pResLineEdit, &QLineEdit::textEdited, this, &CalibrationDialog::resistenceValueChangedSlot);
        //Slope
        QLineEdit* pSlopeLineEdit = new QLineEdit(QString::number(slop, 'g', 10), this);
        m_ptawCalib->setCellWidget(i, 2, pSlopeLineEdit);
        connect(pSlopeLineEdit, &QLineEdit::textEdited, this, &CalibrationDialog::slopeValueChangedSlot);
        //Offset
        QLineEdit* pOffsetLineEdit = new QLineEdit(QString::number(offs, 'g', 10), this);
        m_ptawCalib->setCellWidget(i, 3, pOffsetLineEdit);
        connect(pOffsetLineEdit, &QLineEdit::textEdited, this, &CalibrationDialog::offsetValueChangedSlot);
        //Formula
        QTableWidgetItem* itemFormula = new QTableWidgetItem(QString("y = %1 * x + %2").arg(slop).arg(offs));
        itemFormula->setBackground(QBrush(QColor(Qt::lightGray)));
        itemFormula->setFlags(itemFormula->flags() & (~Qt::ItemIsEditable));
        m_ptawCalib->setItem(i, 4, itemFormula);

    }

    //connect(m_ptawCalib, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(calibValueChangedSlot()));

    return false;
}

bool CalibrationDialog::LoadConfig(const char* pconfig, QSettings** psets)
{
    std::filesystem::path configpath(pconfig);
    if (std::filesystem::exists(configpath))
    {
        *psets = new QSettings(pconfig, QSettings::IniFormat, this);
        auto pset = *psets;

        pset->beginGroup("calibration");
        if (!pset->contains("maxVoltage") || !pset->contains("stepVoltage") || !pset->contains("currentVoltage"))
        {
            //QMessageBox::critical(this, "LoadingConfig", "Invalid Config Loading!!!");
            m_dMaxVolt = 0.19;
            m_dStepVolt = 0.01;
            m_dCurVolt = 0.0;
        }
        else
        {
            m_dMaxVolt = pset->value("maxVoltage").toString().toDouble();
            m_dStepVolt = pset->value("stepVoltage").toString().toDouble();
            m_dCurVolt = pset->value("currentVoltage").toString().toDouble();
        }
        //auto chanNum = pset->value("chanNum").toString().toInt();
        pset->endGroup();

        //m_mapCalib.clear();
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            pset->beginGroup(QString("ch%1").arg(i));
            if (!pset->contains("Resistence") || !pset->contains("Slop") || !pset->contains("Offset"))
            {
                //QMessageBox::critical(this, "LoadingConfig", "Invalid Config Loading2!!!");

                m_vetCalib[i] = { 1000.0, 2.0, 0.0 };
            }
            else
            {
                auto res = pset->value("Resistence").toDouble();
                auto slo = pset->value("Slop").toDouble();
                auto off = pset->value("Offset").toDouble();

                m_vetCalib[i] = { res, slo, off };

            }
            pset->endGroup();
        }

        //QSettings setting("etc/Conf/caliconf.ini", QSettings::IniFormat, this);
        //setting.beginGroup("Calibration");
        //setting.beginReadArray("Channels");
        //for (int i = 0; i < CHANNEL_NUM; ++i)
        //{
        //    setting.setArrayIndex(i);
        //    auto res = setting.value("Resistence").toDouble();
        //    auto slo = setting.value("Slop").toDouble();
        //    auto off = setting.value("Offset").toDouble();
        //
        //    m_vetCalib[i] = { res, slo, off };
        //}
        //setting.endArray();
        //setting.endGroup();

        return true;
    }

    return false;
}

bool CalibrationDialog::SaveConfig(QSettings* pset)
{
    //auto pset = m_pSettingConfig;
    if (pset != nullptr)
    {
        pset->beginGroup("calibration");

        pset->setValue("maxVoltage", m_dMaxVolt);
        pset->setValue("stepVoltage", m_dStepVolt);
        pset->setValue("currentVoltage", m_dCurVolt);

        pset->endGroup();

        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            pset->beginGroup(QString("ch%1").arg(i));

            const auto& calib = m_vetCalib[i];//m_mapCalib[i];
            auto resi = std::get<0>(calib);
            auto slop = std::get<1>(calib);
            auto offs = std::get<2>(calib);
            pset->setValue("Resistence", resi);
            pset->setValue("Slop", slop);
            pset->setValue("Offset", offs);
            pset->setValue("Formula", m_ptawCalib->item(i, 4)->text());

            pset->endGroup();
        }

        return true;
    }

    return false;
}

bool CalibrationDialog::SetConfig2UI(void)
{
    m_pdspMaxVolt->setValue(m_dMaxVolt);
    m_pdspStepVolt->setValue(m_dStepVolt);
    m_pdspCurVolt->setValue(m_dCurVolt);

    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        const auto& calib = m_vetCalib[i];// m_mapCalib[i];
        auto resi = std::get<0>(calib);
        auto slop = std::get<1>(calib);
        auto offs = std::get<2>(calib);
        static_cast<QLineEdit*>(m_ptawCalib->cellWidget(i, 1))->setText(QString::number(resi, 'f', 3));
        static_cast<QLineEdit*>(m_ptawCalib->cellWidget(i, 2))->setText(QString::number(slop, 'g', 10));
        static_cast<QLineEdit*>(m_ptawCalib->cellWidget(i, 3))->setText(QString::number(offs, 'g', 10));
        auto txt = QString("y = %1 * x + %2").arg(slop, 0, 'g', 10).arg(offs, 0, 'g', 10);
        m_ptawCalib->item(i, 4)->setText(txt);
    }
    return false;
}

bool CalibrationDialog::SetUI2Config(void)
{
    m_dMaxVolt = m_pdspMaxVolt->value();
    m_dStepVolt = m_pdspStepVolt->value();
    m_dCurVolt = m_pdspCurVolt->value();

    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        auto resi = static_cast<QLineEdit*>(m_ptawCalib->cellWidget(i, 1))->text();
        auto slop = static_cast<QLineEdit*>(m_ptawCalib->cellWidget(i, 2))->text();
        auto offs = static_cast<QLineEdit*>(m_ptawCalib->cellWidget(i, 3))->text();
        m_vetCalib[i] = { resi.toDouble(), slop.toDouble(), offs.toDouble() };
    }
    return false;
}

void CalibrationDialog::ClearPointBuf(void)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        m_vetPoints[i].clear();
    }
}

void CalibrationDialog::GetAllLineLeastSquare(void)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        double slope = 1.0;
        double offset = 0.0;
        ConfigServer::GetCalibrationLineFit(m_vetPoints[i], slope, offset);
        auto& tpcali = m_vetCalib[i];
        std::get<1>(tpcali) = slope;
        std::get<2>(tpcali) = offset;

        //m_vetPoints[i].clear();
    }
    ClearPointBuf();
}

void CalibrationDialog::closeEvent(QCloseEvent* evn)
{
    if (m_bChanged)
    {
        auto rsp = QMessageBox::question(this, tr("Save Calibration Changes"), tr("The param had been modified.\nDo you want to save your changes?"));
        if (rsp == QMessageBox::Yes)
        {
            SetUI2Config();
            SaveConfig(m_pSettingConfig);
        }
        else
        {
            SetConfig2UI();
        }
        m_bChanged = false;
    }
}

void CalibrationDialog::OnClickAutoCalibration(void)
{
    //QMessageBox::information(this, "AutoCalibration", "You Clicked AutoCalibration!!!");
    m_pbtnAutoCalib->setEnabled(false);
    ConfigServer::GetInstance()->SetCalibration(true);

    m_dMaxVolt = m_pdspMaxVolt->value();
    m_dCurVolt = m_pdspCurVolt->value();
    m_dStepVolt = m_pdspStepVolt->value();
    int counts = (m_dMaxVolt - m_dCurVolt) / m_dStepVolt + 1;

    QProgressDialog progress(this);
    progress.setWindowTitle("AutoCalibrating....");
    progress.setLabelText("Loadding....");
    progress.setCancelButtonText("Cancel");
    progress.setModal(true);//
    progress.show();
    progress.setValue(0);
    progress.setRange(0, counts);//

    if (m_pactStart->isChecked())
    {
        m_pactStart->trigger();
    }
    ConfigServer::caliSleep(2000);

    ClearPointBuf();

    bool bCalibration = true;

    for (int i = 0; i < counts; ++i)
    {
        if (progress.wasCanceled())
        {
            m_pbtnAutoCalib->setEnabled(true);
            bCalibration = false;
            //QMessageBox::information(this, tr("Failed"), tr("Calibration Canceled"));
            break;
        }

        emit SetAllChannelVoltSignal(m_dCurVolt);
        if (!m_pactStart->isChecked())
        {
            m_pactStart->trigger();
        }
        progress.setValue(i);
        ConfigServer::caliSleep(3000);

        if (m_pactStart->isChecked())
        {
            m_pactStart->trigger();
        }
        ConfigServer::caliSleep(3000);

        m_dCurVolt += m_dStepVolt;
    }

    if (bCalibration)
    {
        GetAllLineLeastSquare();
        progress.setValue(counts);

        m_dCurVolt = 0.0;
        SetConfig2UI();
        SaveConfig(m_pSettingConfig);
        m_bChanged = false;

        QMessageBox::information(this, tr("Info"), tr("Calibration Success"));
    }

    ConfigServer::GetInstance()->SetCalibration(false);

    m_pbtnAutoCalib->setEnabled(true);
}

void CalibrationDialog::OnClickLoadConfig(void)
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Config File", "", "Config.ini(*.ini)");
    if (!fileName.isEmpty())
    {
        QSettings* psets = nullptr;
        LoadConfig(fileName.toStdString().c_str(), &psets);
        SetConfig2UI();
        SAFE_DELETEP(psets);
    }
}

void CalibrationDialog::OnClickAllResistance(void)
{
    double resi = m_pdspAllResistance->value();
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        auto& cali = m_vetCalib[i];
        std::get<0>(cali) = resi;
        static_cast<QLineEdit*>(m_ptawCalib->cellWidget(i, 1))->setText(QString::number(resi, 'f', 3));
    }
    m_bChanged = true;
}

void CalibrationDialog::OnClickAllSlope(void)
{
    double slop = m_pdspAllSlope->value();
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        auto& cali = m_vetCalib[i];
        std::get<1>(cali) = slop;
        static_cast<QLineEdit*>(m_ptawCalib->cellWidget(i, 2))->setText(QString::number(slop, 'g', 10));
        m_ptawCalib->item(i, 4)->setText(QString("y = %1 * x + %2").arg(slop, 0, 'g', 10).arg(std::get<2>(cali), 0, 'g', 10));
    }
    m_bChanged = true;
}

void CalibrationDialog::OnClickAllOffset(void)
{
    double offs = m_pdspAllOffset->value();
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        auto& cali = m_vetCalib[i];
        std::get<2>(cali) = offs;
        static_cast<QLineEdit*>(m_ptawCalib->cellWidget(i, 3))->setText(QString::number(offs, 'g', 10));
        m_ptawCalib->item(i, 4)->setText(QString("y = %1 * x + %2").arg(std::get<1>(cali), 0, 'g', 10).arg(offs, 0, 'g', 10));
    }
    m_bChanged = true;
}

void CalibrationDialog::OnClickSaveChange(void)
{
    //if (m_bChanged)
    {
        m_bChanged = false;
        SetUI2Config();
        SaveConfig(m_pSettingConfig);
    }
}

void CalibrationDialog::OnClickClearChange(void)
{
    if (m_bChanged)
    {
        auto rsp = QMessageBox::question(this, tr("Clear Calibration Changes"), tr("The param had been modified.\nDo you want to clear your changes?"));
        if (rsp == QMessageBox::Yes)
        {
            m_bChanged = false;

            SetConfig2UI();
        }
    }
}

void CalibrationDialog::resistenceValueChangedSlot(QString str)
{
    int row = m_ptawCalib->currentRow();
    if (row >= 0)
    {
        m_bChanged = true;
        LOGI("textChanged={}!!!={}", __FUNCTION__, row);
    }
}

void CalibrationDialog::slopeValueChangedSlot(QString str)
{
    int row = m_ptawCalib->currentRow();
    if (row >= 0)
    {
        m_bChanged = true;
        auto off = static_cast<QLineEdit*>(m_ptawCalib->cellWidget(row, 3))->text().toDouble();
        m_ptawCalib->item(row, 4)->setText(QString("y = %1 * x + %2").arg(str.toDouble(), 0, 'g', 10).arg(off, 0, 'g', 10));
    }
}

void CalibrationDialog::offsetValueChangedSlot(QString str)
{
    int row = m_ptawCalib->currentRow();
    if (row >= 0)
    {
        m_bChanged = true;
        auto slop = static_cast<QLineEdit*>(m_ptawCalib->cellWidget(row, 2))->text().toDouble();
        m_ptawCalib->item(row, 4)->setText(QString("y = %1 * x + %2").arg(slop, 0, 'g', 10).arg(str.toDouble(), 0, 'g', 10));
    }
}

void CalibrationDialog::calibValueChangedSlot(void)
{
    m_bChanged = true;
    LOGI("textChanged={}!!!", __FUNCTION__);
}

void CalibrationDialog::setCalibrationSlot(const float* pavg)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        double dx = m_dCurVolt * 1000000.0 / std::get<0>(m_vetCalib[i]);
        auto& dy = pavg[i];
        m_vetPoints[i].emplace_back(dx, dy);
    }
}
