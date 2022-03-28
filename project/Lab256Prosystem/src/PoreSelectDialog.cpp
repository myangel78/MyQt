#include "PoreSelectDialog.h"
#include <qlayout.h>
#include <qmessagebox.h>
#include <filesystem>
#include <Log.h>

#include "ConfigServer.h"
#include "ControlView.h"


PoreSelectDialog::PoreSelectDialog(QWidget *parent) : QDialog(parent)
{
	InitCtrl();
	std::string strpsconfig = ConfigServer::GetUserPath() + "/etc/Conf/psConfig.ini";
	InitConfig(strpsconfig.c_str());

	m_tmrStartMode.setSingleShot(true);
	connect(&m_tmrStartMode, SIGNAL(timeout()), this, SLOT(OnTimerStartModeSlot()));

	connect(&m_tmrPoreFilter, SIGNAL(timeout()), this, SLOT(TimerPoreFilterSlot()));
}

bool PoreSelectDialog::InitCtrl(void)
{
	{
		m_pgrpMakeMembrance = new QGroupBox("MakeMembrane", this);

		for (int i = 0; i < m_sciFlow; ++i)
		{
			m_pchkMakeMembrance[i] = new QCheckBox(QString::number(i + 1), m_pgrpMakeMembrance);

			m_pdspMakeMembrance[i] = new QDoubleSpinBox(m_pgrpMakeMembrance);
			m_pdspMakeMembrance[i]->setDecimals(2);
			m_pdspMakeMembrance[i]->setSuffix(" V");
			m_pdspMakeMembrance[i]->setSingleStep(0.1);
			m_pdspMakeMembrance[i]->setMinimum(-0.4);
			m_pdspMakeMembrance[i]->setMaximum(0.4);

			m_pispMakeMembrance[i] = new QSpinBox(m_pgrpMakeMembrance);
			m_pispMakeMembrance[i]->setSuffix(" s");
			m_pispMakeMembrance[i]->setMaximum(21600);//6h
		}
		m_plabMakeMembranceNum = new QLabel("Loop Count:", m_pgrpMakeMembrance);
		m_pispMakeMembranceNum = new QSpinBox(m_pgrpMakeMembrance);
		m_pispMakeMembranceNum->setMinimum(1);


		QGridLayout* gdlayout1 = new QGridLayout();
		for (int i = 0; i < m_sciFlow; ++i)
		{
			int col = 0;
			gdlayout1->addWidget(m_pchkMakeMembrance[i], i, col++);
			gdlayout1->addWidget(m_pdspMakeMembrance[i], i, col++);
			gdlayout1->addWidget(m_pispMakeMembrance[i], i, col++);
		}

		QHBoxLayout* horlayout1 = new QHBoxLayout();
		horlayout1->addWidget(m_plabMakeMembranceNum);
		horlayout1->addWidget(m_pispMakeMembranceNum);

		QVBoxLayout* verlayout1 = new QVBoxLayout();
		verlayout1->addLayout(gdlayout1);
		verlayout1->addLayout(horlayout1);

		m_pgrpMakeMembrance->setLayout(verlayout1);

	}

	{
		m_pgrpPoreInsert = new QGroupBox("PoreInsert", this);

		for (int i = 0; i < m_sciFlow; ++i)
		{
			m_pchkPoreInsert[i] = new QCheckBox(QString::number(i + 1), m_pgrpPoreInsert);

			m_pdspPoreInsert[i] = new QDoubleSpinBox(m_pgrpPoreInsert);
			m_pdspPoreInsert[i]->setDecimals(2);
			m_pdspPoreInsert[i]->setSuffix(" V");
			m_pdspPoreInsert[i]->setSingleStep(0.1);
			m_pdspPoreInsert[i]->setMinimum(-0.4);
			m_pdspPoreInsert[i]->setMaximum(0.4);

			m_pispPoreInsert[i] = new QSpinBox(m_pgrpPoreInsert);
			m_pispPoreInsert[i]->setSuffix(" s");
			m_pispPoreInsert[i]->setMaximum(21600);//6h
		}
		m_plabPoreInsertNum = new QLabel("Loop Count:", m_pgrpPoreInsert);
		m_pispPoreInsertNum = new QSpinBox(m_pgrpPoreInsert);
		m_pispPoreInsertNum->setMinimum(1);


		QGridLayout* gdlayout1 = new QGridLayout();
		for (int i = 0; i < m_sciFlow; ++i)
		{
			int col = 0;
			gdlayout1->addWidget(m_pchkPoreInsert[i], i, col++);
			gdlayout1->addWidget(m_pdspPoreInsert[i], i, col++);
			gdlayout1->addWidget(m_pispPoreInsert[i], i, col++);
		}

		QHBoxLayout* horlayout1 = new QHBoxLayout();
		horlayout1->addWidget(m_plabPoreInsertNum);
		horlayout1->addWidget(m_pispPoreInsertNum);

		QVBoxLayout* verlayout1 = new QVBoxLayout();
		verlayout1->addLayout(gdlayout1);
		verlayout1->addLayout(horlayout1);

		m_pgrpPoreInsert->setLayout(verlayout1);
	}

	{
		m_pgrpChannelSel = new QGroupBox("ChannelFilter", this);
		m_pgrpChannelSel->setCheckable(true);
		m_pgrpChannelSel->setChecked(false);

		for (int i = 0; i < m_sciFlow; ++i)
		{
			m_pchkChannelSel[i] = new QCheckBox(QString::number(i + 1), m_pgrpChannelSel);

			m_pdspChannelSel[i] = new QDoubleSpinBox(m_pgrpChannelSel);
			m_pdspChannelSel[i]->setDecimals(2);
			m_pdspChannelSel[i]->setSuffix(" V");
			m_pdspChannelSel[i]->setSingleStep(0.1);
			m_pdspChannelSel[i]->setMinimum(-0.4);
			m_pdspChannelSel[i]->setMaximum(0.4);

			m_pispChannelSel[i] = new QSpinBox(m_pgrpChannelSel);
			m_pispChannelSel[i]->setSuffix(" s");
			m_pispChannelSel[i]->setMaximum(21600);//6h
		}
		m_pdspCurrentMin = new QDoubleSpinBox(m_pgrpChannelSel);
		m_pdspCurrentMin->setDecimals(2);
		m_pdspCurrentMin->setSuffix("pA");
		m_pdspCurrentMin->setMinimum(-9999.99);
		m_pdspCurrentMin->setMaximum(9999.99);

		m_pdspCurrentMax = new QDoubleSpinBox(m_pgrpChannelSel);
		m_pdspCurrentMax->setDecimals(2);
		m_pdspCurrentMax->setSuffix("pA");
		m_pdspCurrentMax->setMinimum(-9999.99);
		m_pdspCurrentMax->setMaximum(9999.99);


		QGridLayout* gdlayout1 = new QGridLayout();
		for (int i = 0; i < m_sciFlow; ++i)
		{
			int col = 0;
			gdlayout1->addWidget(m_pchkChannelSel[i], i, col++);
			gdlayout1->addWidget(m_pdspChannelSel[i], i, col++);
			gdlayout1->addWidget(m_pispChannelSel[i], i, col++);
		}

		QHBoxLayout* horlayout1 = new QHBoxLayout();
		horlayout1->addWidget(m_pdspCurrentMin);
		horlayout1->addWidget(m_pdspCurrentMax);

		QVBoxLayout* verlayout1 = new QVBoxLayout();
		verlayout1->addLayout(gdlayout1);
		verlayout1->addLayout(horlayout1);
		//verlayout1->addStretch(1);

		m_pgrpChannelSel->setLayout(verlayout1);
	}

	{
		m_pgrpFinishedVolt = new QGroupBox("FinishedVoltSet", this);

		m_plabFinishedVolt = new QLabel("Finished Voltage:", m_pgrpFinishedVolt);
		m_pdspFinishedVolt = new QDoubleSpinBox(m_pgrpFinishedVolt);
		m_pdspFinishedVolt->setDecimals(2);
		m_pdspFinishedVolt->setSuffix(" V");
		m_pdspFinishedVolt->setSingleStep(0.1);
		m_pdspFinishedVolt->setMinimum(-0.4);
		m_pdspFinishedVolt->setMaximum(0.4);


		QHBoxLayout* horlayout1 = new QHBoxLayout();
		horlayout1->addStretch(1);
		horlayout1->addWidget(m_plabFinishedVolt);
		horlayout1->addWidget(m_pdspFinishedVolt);
		horlayout1->addStretch(5);

		m_pgrpFinishedVolt->setLayout(horlayout1);
	}

	{
		m_pgrpOperation = new QGroupBox("Operation", this);

		m_peditModeName = new QLineEdit("Mode Name", m_pgrpOperation);

		m_pcmbModeName = new QComboBox(m_pgrpOperation);
		m_pcmbModeName->setMinimumWidth(150);

		m_pbtnAddMode = new QPushButton("Add Mode", m_pgrpOperation);
		m_pbtnDelMode = new QPushButton("Del Mode", m_pgrpOperation);
		m_pbtnEditMode = new QPushButton("Edit Mode", m_pgrpOperation);
		m_pbtnEditMode->setVisible(false);
		m_pbtnStartMode = new QPushButton("Start Mode", m_pgrpOperation);

		m_pbtnStartMode->setIcon(QIcon(":/img/img/run.png"));
		m_pbtnStartMode->setIconSize(QSize(30, 30));
		m_pbtnStartMode->setCheckable(true);


		int col = 0;
		QGridLayout* gdlayout1 = new QGridLayout();
		gdlayout1->addWidget(m_peditModeName, 0, col++);
		gdlayout1->addWidget(m_pcmbModeName, 0, col++);
		gdlayout1->addWidget(m_pbtnAddMode, 0, col++);
		gdlayout1->addWidget(m_pbtnDelMode, 0, col++);
		gdlayout1->addWidget(m_pbtnEditMode, 0, col++);
		gdlayout1->addWidget(m_pbtnStartMode, 0, col++);

		m_pgrpOperation->setLayout(gdlayout1);
	}


	QHBoxLayout* horlayout9 = new QHBoxLayout();
	horlayout9->addWidget(m_pgrpMakeMembrance);
	horlayout9->addWidget(m_pgrpPoreInsert);
	horlayout9->addWidget(m_pgrpChannelSel);

	QVBoxLayout* verlayout9 = new QVBoxLayout();
	verlayout9->addLayout(horlayout9);
	verlayout9->addWidget(m_pgrpFinishedVolt);
	verlayout9->addWidget(m_pgrpOperation);

	setLayout(verlayout9);


	connect(m_pcmbModeName, SIGNAL(currentIndexChanged(int)), this, SLOT(OnChangeModeName(int)));
	connect(m_pbtnAddMode, &QPushButton::clicked, this, &PoreSelectDialog::OnClickAddMode);
	connect(m_pbtnDelMode, &QPushButton::clicked, this, &PoreSelectDialog::OnClickDelMode);
	connect(m_pbtnEditMode, &QPushButton::clicked, this, &PoreSelectDialog::OnClickEditMode);
	connect(m_pbtnStartMode, &QPushButton::clicked, this, &PoreSelectDialog::OnClickStartMode);

	return false;
}

bool PoreSelectDialog::InitConfig(const char* pfile)
{
	//if (!std::filesystem::exists(pfile))
	//{
	//	LOGI("No mode config Found!!!");
	//	return false;
	//}
	m_pConfig = new QSettings(pfile, QSettings::IniFormat, this);
	return LoadConfig(m_pConfig);
}

void PoreSelectDialog::closeEvent(QCloseEvent* evn)
{
	emit EnablePoreInsertSignal(!m_bRunning);
}

bool PoreSelectDialog::LoadConfig(QSettings* pset)
{
	if (pset != nullptr)
	{
		pset->beginGroup("PoreSelect");
		int modecnt = pset->value("modenum").toInt();
		pset->endGroup();

		m_pcmbModeName->clear();
		m_vetPoreSelectData.clear();
		for (int j = 0; j < modecnt; ++j)
		{
			SPoreSelectData psdata;

			auto modeno = QString("PoreSelectMode%1").arg(j);
			pset->beginGroup(modeno);

			for (int i = 0; i < m_sciFlow; ++i)
			{
				SVoltTime svt1;
				svt1.selchk = pset->value(QString("mmchk%1").arg(i)).toBool();
				svt1.selvolt = pset->value(QString("mmvolt%1").arg(i)).toDouble();
				svt1.seltime = pset->value(QString("mmtime%1").arg(i)).toInt();
				psdata.voltTime1.emplace_back(svt1);

				SVoltTime svt2;
				svt2.selchk = pset->value(QString("pichk%1").arg(i)).toBool();
				svt2.selvolt = pset->value(QString("pivolt%1").arg(i)).toDouble();
				svt2.seltime = pset->value(QString("pitime%1").arg(i)).toInt();
				psdata.voltTime2.emplace_back(svt2);

				SVoltTime svt3;
				svt3.selchk = pset->value(QString("cschk%1").arg(i)).toBool();
				svt3.selvolt = pset->value(QString("csvolt%1").arg(i)).toDouble();
				svt3.seltime = pset->value(QString("cstime%1").arg(i)).toInt();
				psdata.voltTime3.emplace_back(svt3);
			}
			psdata.modeName = pset->value("modename").toString().toStdString();
			psdata.loopCount1 = pset->value("mmnum").toInt();
			psdata.loopCount2 = pset->value("pinum").toInt();
			psdata.filterchk = pset->value("csenable").toBool();
			psdata.minFilter = pset->value("csmin").toDouble();
			psdata.maxFilter = pset->value("csmax").toDouble();
			psdata.finvolt = pset->value("finvolt").toDouble();

			pset->endGroup();

			m_vetPoreSelectData.emplace_back(psdata);

			m_pcmbModeName->addItem(psdata.modeName.c_str());
		}

		return true;
	}
	return false;
}

bool PoreSelectDialog::SaveConfig(QSettings* pset)
{
	if (pset != nullptr)
	{
		int modecnt = m_vetPoreSelectData.size();
		pset->beginGroup("PoreSelect");
		pset->setValue("modenum", modecnt);
		pset->endGroup();

		for (int j = 0; j < modecnt; ++j)
		{
			auto modeno = QString("PoreSelectMode%1").arg(j);
			pset->beginGroup(modeno);

			const auto& psdata = m_vetPoreSelectData[j];
			for (int i = 0; i < m_sciFlow; ++i)
			{
				pset->setValue(QString("mmchk%1").arg(i), psdata.voltTime1[i].selchk);
				pset->setValue(QString("mmvolt%1").arg(i), psdata.voltTime1[i].selvolt);
				pset->setValue(QString("mmtime%1").arg(i), psdata.voltTime1[i].seltime);

				pset->setValue(QString("pichk%1").arg(i), psdata.voltTime2[i].selchk);
				pset->setValue(QString("pivolt%1").arg(i), psdata.voltTime2[i].selvolt);
				pset->setValue(QString("pitime%1").arg(i), psdata.voltTime2[i].seltime);

				pset->setValue(QString("cschk%1").arg(i), psdata.voltTime3[i].selchk);
				pset->setValue(QString("csvolt%1").arg(i), psdata.voltTime3[i].selvolt);
				pset->setValue(QString("cstime%1").arg(i), psdata.voltTime3[i].seltime);
			}
			pset->setValue("modename", psdata.modeName.c_str());
			pset->setValue("mmnum", psdata.loopCount1);
			pset->setValue("pinum", psdata.loopCount2);
			pset->setValue("csenable", psdata.filterchk);
			pset->setValue("csmin", psdata.minFilter);
			pset->setValue("csmax", psdata.maxFilter);
			pset->setValue("finvolt", psdata.finvolt);

			pset->endGroup();
		}

		return true;
	}
	return false;
}

void PoreSelectDialog::OnChangeModeName(int val)
{
	//QString mode = m_pcmbModeName->itemText(val);
	if (val >= m_vetPoreSelectData.size())
	{
		return;
	}
	auto& item = m_vetPoreSelectData[val];
	for (int i = 0; i < m_sciFlow; ++i)
	{
		auto& svt1 = item.voltTime1[i];
		m_pchkMakeMembrance[i]->setChecked(svt1.selchk);
		m_pdspMakeMembrance[i]->setValue(svt1.selvolt);
		m_pispMakeMembrance[i]->setValue(svt1.seltime);

		auto& svt2 = item.voltTime2[i];
		m_pchkPoreInsert[i]->setChecked(svt2.selchk);
		m_pdspPoreInsert[i]->setValue(svt2.selvolt);
		m_pispPoreInsert[i]->setValue(svt2.seltime);

		auto& svt3 = item.voltTime3[i];
		m_pchkChannelSel[i]->setChecked(svt3.selchk);
		m_pdspChannelSel[i]->setValue(svt3.selvolt);
		m_pispChannelSel[i]->setValue(svt3.seltime);
	}
	m_pispMakeMembranceNum->setValue(item.loopCount1);
	m_pispPoreInsertNum->setValue(item.loopCount2);
	m_pgrpChannelSel->setChecked(item.filterchk);
	m_pdspCurrentMin->setValue(item.minFilter);
	m_pdspCurrentMax->setValue(item.maxFilter);
	m_pdspFinishedVolt->setValue(item.finvolt);
}

void PoreSelectDialog::OnClickAddMode(void)
{
	int cnt1 = 0;
	int cnt2 = 0;
	for (int i = 0; i < m_sciFlow; ++i)
	{
		if (m_pchkMakeMembrance[i]->isChecked())
		{
			if (m_pispMakeMembrance[i]->value() == 0)
			{
				//wrong param.
				QMessageBox::warning(this, "Wrong MakeMembrane Param", "MakeMembrane Checked param wrong!!!");
				return;
			}
			++cnt1;
		}
		if (m_pchkPoreInsert[i]->isChecked())
		{
			if (m_pispPoreInsert[i]->value() == 0)
			{
				//wrong param.
				QMessageBox::warning(this, "Wrong PoreInsert Param", "PoreInsert Checked param wrong!!!");
				return;
			}
			++cnt2;
		}
	}
	if (cnt1 == 0)
	{
		//no check.
		QMessageBox::warning(this, "No MakeMembrane Checked", "MakeMembrane No Item Checked!!!");
		return;
	}
	if (cnt2 == 0)
	{
		//no check.
		QMessageBox::warning(this, "No PoreInsert Checked", "PoreInsert No Item Checked!!!");
		return;
	}
	if (m_pgrpChannelSel->isChecked())
	{
		int cnt3 = 0;
		for (int i = 0; i < m_sciFlow; ++i)
		{
			if (m_pchkChannelSel[i]->isChecked())
			{
				if (m_pispChannelSel[i]->value() == 0)
				{
					//wrong param.
					QMessageBox::warning(this, "Wrong ChannelFilter Param", "ChannelFilter Checked param wrong!!!");
					return;
				}
				++cnt3;
			}
		}
		if (cnt3 == 0)
		{
			//no check.
			QMessageBox::warning(this, "No ChannelFilter Checked", "ChannelFilter No Item Checked!!!");
			return;
		}
	}

	QString name = m_peditModeName->text();
	if (name.isEmpty())
	{
		//name is empty.
		QMessageBox::warning(this, "Name Empty", "Mode Name Empty!!!");
		return;
	}
	for (int i = 0, n = m_pcmbModeName->count(); i < n; ++i)
	{
		QString item = m_pcmbModeName->itemText(i);
		if (item == name)
		{
			//duplicate name.
			QMessageBox::warning(this, "Duplicate Name", "Duplicate Mode Name!!!");
			return;
		}
	}

	SPoreSelectData psdata;
	for (int i = 0; i < m_sciFlow; ++i)
	{
		SVoltTime svt1;
		svt1.selchk = m_pchkMakeMembrance[i]->isChecked();
		svt1.selvolt = m_pdspMakeMembrance[i]->value();
		svt1.seltime = m_pispMakeMembrance[i]->value();
		psdata.voltTime1.emplace_back(svt1);

		SVoltTime svt2;
		svt2.selchk = m_pchkPoreInsert[i]->isChecked();
		svt2.selvolt = m_pdspPoreInsert[i]->value();
		svt2.seltime = m_pispPoreInsert[i]->value();
		psdata.voltTime2.emplace_back(svt2);

		SVoltTime svt3;
		svt3.selchk = m_pchkChannelSel[i]->isChecked();
		svt3.selvolt = m_pdspChannelSel[i]->value();
		svt3.seltime = m_pispChannelSel[i]->value();
		psdata.voltTime3.emplace_back(svt3);
	}
	psdata.modeName = name.toStdString();
	psdata.loopCount1 = m_pispMakeMembranceNum->value();
	psdata.loopCount2 = m_pispPoreInsertNum->value();
	psdata.filterchk = m_pgrpChannelSel->isChecked();
	psdata.minFilter = m_pdspCurrentMin->value();
	psdata.maxFilter = m_pdspCurrentMax->value();
	psdata.finvolt = m_pdspFinishedVolt->value();

	m_vetPoreSelectData.emplace_back(psdata);

	m_pcmbModeName->addItem(name);
	m_pcmbModeName->setCurrentIndex(m_pcmbModeName->count() - 1);

	SaveConfig(m_pConfig);
}

void PoreSelectDialog::OnClickDelMode(void)
{
	if (m_pcmbModeName->count() <= 0)
	{
		return;
	}
	int cur = m_pcmbModeName->currentIndex();
	if (cur >= m_vetPoreSelectData.size())
	{
		return;
	}
	m_vetPoreSelectData.erase(m_vetPoreSelectData.begin() + cur);
	m_pcmbModeName->removeItem(cur);

	SaveConfig(m_pConfig);
}

void PoreSelectDialog::OnClickEditMode(void)
{
	if (m_pcmbModeName->count() <= 0)
	{
		return;
	}
	int cur = m_pcmbModeName->currentIndex();
	if (cur >= m_vetPoreSelectData.size())
	{
		return;
	}
	auto& item = m_vetPoreSelectData[cur];
	for (int i = 0; i < m_sciFlow; ++i)
	{
		auto& vt1 = item.voltTime1[i];
		vt1.selchk = m_pchkMakeMembrance[i]->isChecked();
		vt1.selvolt = m_pdspMakeMembrance[i]->value();
		vt1.seltime = m_pispMakeMembrance[i]->value();

		auto& vt2 = item.voltTime2[i];
		vt2.selchk = m_pchkPoreInsert[i]->isChecked();
		vt2.selvolt = m_pdspPoreInsert[i]->value();
		vt2.seltime = m_pispPoreInsert[i]->value();

		auto& vt3 = item.voltTime2[i];
		vt3.selchk = m_pchkPoreInsert[i]->isChecked();
		vt3.selvolt = m_pdspPoreInsert[i]->value();
		vt3.seltime = m_pispPoreInsert[i]->value();
	}
	item.loopCount1 = m_pispMakeMembranceNum->value();
	item.loopCount2 = m_pispPoreInsertNum->value();
	item.filterchk = m_pgrpChannelSel->isChecked();
	item.minFilter = m_pdspCurrentMin->value();
	item.maxFilter = m_pdspCurrentMax->value();
	item.finvolt = m_pdspFinishedVolt->value();

	SaveConfig(m_pConfig);
}

void PoreSelectDialog::OnClickStartMode(bool chk)
{
	if (chk)
	{
		if (m_bNeedZeroAdjust)
		{
			m_pControlView->onWaitingZeroAdjust();
		}
		m_bRunning = true;
		ConfigServer::GetInstance()->SetRunSleep(true);
		m_tmrStartMode.start(500);

		m_pbtnStartMode->setIcon(QIcon(":/img/img/stop.png"));
		m_pbtnStartMode->setText("Stop Mode");
	}
	else
	{
		m_bRunning = false;
		ConfigServer::GetInstance()->SetRunSleep(false);

		m_pbtnStartMode->setIcon(QIcon(":/img/img/run.png"));
		m_pbtnStartMode->setText("Start Mode");
	}
}

void PoreSelectDialog::OnTimerStartModeSlot(void)
{
	int cnt1 = m_pispMakeMembranceNum->value();
	for (int j = cnt1; j > 0 && m_bRunning; --j)
	{
		for (int i = 0; i < m_sciFlow && m_bRunning; ++i)
		{
			if (m_pchkMakeMembrance[i]->isChecked())
			{
				emit SetSelChannelDCLevelSignal(m_pdspMakeMembrance[i]->value());
				ConfigServer::GetInstance()->PoreSleep(m_pispMakeMembrance[i]->value() * 1000);
			}
		}
	}
	if (!m_bRunning)
	{
		return;
	}
	auto ret = QMessageBox::information(this, "MakeMembrane Finished", "MakeMembrane Finished, Please Add Protein to Next Step");
	if (ret != QMessageBox::Ok)
	{
		return;
	}

	if (m_bRunning && m_pgrpChannelSel->isChecked())
	{
		m_tmrPoreFilter.start(200);
	}
	int cnt2 = m_pispPoreInsertNum->value();
	for (int j = cnt2; j > 0 && m_bRunning; --j)
	{
		for (int i = 0; i < m_sciFlow && m_bRunning; ++i)
		{
			if (m_pchkPoreInsert[i]->isChecked())
			{
				emit SetNotPoreFilterDCLevelSignal(m_pdspPoreInsert[i]->value());
				ConfigServer::GetInstance()->PoreSleep(m_pispPoreInsert[i]->value() * 1000);
			}
		}
	}
	if (m_tmrPoreFilter.isActive())
	{
		m_tmrPoreFilter.stop();
	}

	emit SetSelChannelDCLevelSignal(m_pdspFinishedVolt->value());

	if (m_pbtnStartMode->isChecked())
	{
		m_pbtnStartMode->click();
	}
}

void PoreSelectDialog::TimerPoreFilterSlot(void)
{
	emit SetPoreFilterChannelSignal(m_pdspCurrentMin->value(), m_pdspCurrentMax->value());
	for (int i = 0; i < m_sciFlow && m_bRunning; ++i)
	{
		if (m_pchkChannelSel[i]->isChecked())
		{
			emit SetPoreFilterDCLevelSignal(m_pdspChannelSel[i]->value());
			ConfigServer::GetInstance()->PoreSleep(m_pispChannelSel[i]->value() * 1000);
		}
	}
}
