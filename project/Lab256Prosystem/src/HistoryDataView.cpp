#include "HistoryDataView.h"
#include <qlayout.h>
#include <Log.h>
#include "UsbProtoMsg.h"
#include "ConfigServer.h"


HistoryListItem::HistoryListItem(int chan, QWidget* parent)
	: QWidget(parent)
	, m_iChannel(chan)
{
	InitCtrl();
}

bool HistoryListItem::InitCtrl(void)
{
	int channo = m_iChannel + 1;
	m_pchkChannel = new QCheckBox(QString("CH%1").arg(channo), this);
	m_pchkChannel->setChecked(true);
	m_pchkChannel->setMinimumWidth(60);
	m_pchkChannel->setMaximumWidth(65);
	m_pchkChannel->setToolTip(QString("Show Channel %1").arg(channo));

	m_pbtnChannel = new QToolButton(this);
	m_pbtnChannel->setFixedSize(QSize(16, 16));
	m_pbtnChannel->setAutoRaise(true);
	QAction* action = new QAction(this);
	action->setToolTip(tr("Plot line color"));
	m_pbtnChannel->setDefaultAction(action);
	QPixmap pixmap(16, 16);
	QPainter painter(&pixmap);
	painter.setPen(QColor(50, 50, 50));
	painter.drawRect(QRect(0, 0, 15, 15));
	painter.fillRect(QRect(1, 1, 14, 14), g_vetPenColor[m_iChannel % 32]);
	m_pbtnChannel->defaultAction()->setIcon(QIcon(pixmap));

	m_plabCurrentValue = new QLabel(this);
	m_plabCurrentValue->setMinimumWidth(50);
	m_plabCurrentValue->setFrameShape(QFrame::StyledPanel);
	m_plabCurrentValue->setToolTip(QString("Current Value for Channel %1").arg(channo));


	QHBoxLayout* horlayout1 = new QHBoxLayout();
	horlayout1->setContentsMargins(1, 0, 1, 0);
	horlayout1->setSpacing(0);
	horlayout1->addWidget(m_pchkChannel);
	horlayout1->addWidget(m_pbtnChannel);
	horlayout1->addWidget(m_plabCurrentValue);

	setLayout(horlayout1);


	connect(m_pchkChannel, &QCheckBox::stateChanged, this, &HistoryListItem::OnCheckShowChanged);

	return true;
}

void HistoryListItem::OnCheckShowChanged(bool chk)
{
	emit checkShowChangedSignal(m_iChannel, chk);
}


HistoryDataView::HistoryDataView(QWidget *parent) : QWidget(parent)
{
	InitCtrl();

	connect(m_pHistoryPlot, &QCustomPlot::mouseMove, this, &HistoryDataView::MyMouseMoveEvent);
}

bool HistoryDataView::InitCtrl(void)
{
	//Plot
	m_pgrpPlot = new QGroupBox(this);

	m_pHistoryPlot = new QCustomPlot(m_pgrpPlot);
	m_pHistoryPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
	m_pHistoryPlot->axisRect()->setupFullAxesBox();
	m_pHistoryPlot->xAxis->setLabel("Time s/ch");
	QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
	timeTicker->setTimeFormat("%h:%m:%s");
	m_pHistoryPlot->xAxis->setTicker(timeTicker);
	connect(m_pHistoryPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), m_pHistoryPlot->xAxis2, SLOT(setRange(QCPRange)));
	connect(m_pHistoryPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), m_pHistoryPlot->yAxis2, SLOT(setRange(QCPRange)));

	//m_pHistoryPlot->addLayer("histlayer1", 0, QCustomPlot::limAbove);
	//m_pHistoryPlot->layer("histlayer1")->setMode(QCPLayer::lmBuffered);
	for (int i = 0; i < CHANNEL_NUM; ++i)
	{
		m_graph = m_pHistoryPlot->addGraph();
		m_graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1), QBrush(Qt::white), 3));
		m_graph->setPen(QPen(g_vetPenColor[i % 32]));
		m_graph->SetLastDataRange(false);
		//m_graph->setLayer("histlayer1");
	}
	m_pHistoryPlot->xAxis->setRange(0.0, 20.0);
	m_pHistoryPlot->yAxis->setRange(0.0, 500.0);

	m_plabYmax = new QLabel(tr("YMax"), this);
	m_pdspYmax = new QDoubleSpinBox(this);
	m_pdspYmax->setMaximum(50000.0);
	m_pdspYmax->setMinimum(-50000.0);
	m_pdspYmax->setValue(500.0);
	m_plabYmin = new QLabel(tr("YMin"), this);
	m_pdspYmin = new QDoubleSpinBox(this);
	m_pdspYmin->setMaximum(50000.0);
	m_pdspYmin->setMinimum(-50000.0);
	m_pdspYmin->setValue(0.0);
	m_plabPtnum = new QLabel(tr("Time"), this);
	m_pdspPtnum = new QDoubleSpinBox(this);
	m_pdspPtnum->setSuffix(" s");
	m_pdspPtnum->setMinimum(0.5);
	m_pdspPtnum->setMaximum(86400.0);//24*3600s
	m_pdspPtnum->setValue(20.0);
	m_pbtnPlotApply = new QPushButton(tr("Apply"), this);
	connect(m_pbtnPlotApply, &QPushButton::clicked, this, &HistoryDataView::OnPlotApply);

	m_peditFileName = new QLineEdit("FileName", m_pgrpPlot);
	m_peditFileName->setReadOnly(true);

	m_pbtnLoadFile = new QPushButton("Load...", m_pgrpPlot);
	m_pbtnLoadFile->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>加载数据csv文件</p></body></html>"));

	m_pbtnReloadFile = new QPushButton("Reload", m_pgrpPlot);
	m_pbtnReloadFile->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>重新加载之前的数据csv文件</p></body></html>"));

	m_pbtnResetData = new QPushButton("Reset", m_pgrpPlot);
	m_pbtnResetData->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>重置当前的数据曲线</p></body></html>"));

	QHBoxLayout* phorlayout1 = new QHBoxLayout();
	phorlayout1->addStretch(1);
	phorlayout1->addWidget(m_plabYmax);
	phorlayout1->addWidget(m_pdspYmax);
	phorlayout1->addWidget(m_plabYmin);
	phorlayout1->addWidget(m_pdspYmin);
	phorlayout1->addWidget(m_plabPtnum);
	phorlayout1->addWidget(m_pdspPtnum);
	phorlayout1->addWidget(m_pbtnPlotApply);
	phorlayout1->addStretch(1);

	QHBoxLayout* horlayout2 = new QHBoxLayout();
	horlayout2->addWidget(m_peditFileName);
	horlayout2->addWidget(m_pbtnLoadFile);
	horlayout2->addWidget(m_pbtnReloadFile);
	horlayout2->addWidget(m_pbtnResetData);

	QVBoxLayout* verlayout1 = new QVBoxLayout();
	verlayout1->addWidget(m_pHistoryPlot);
	verlayout1->addLayout(phorlayout1);
	verlayout1->addLayout(horlayout2);

	m_pgrpPlot->setLayout(verlayout1);


	//Ctrl
	m_pgrpCtrl = new QGroupBox(this);
	m_pgrpCtrl->setMaximumWidth(200);

	m_pispTimeNode = new QSpinBox(m_pgrpCtrl);
	m_pispTimeNode->setSuffix(" min");
	m_pispTimeNode->setMinimum(-1);
	m_pispTimeNode->setMaximum(1439);
	m_pispTimeNode->setValue(-1);
	m_pispTimeNode->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置加载数据时间长度，最小为-1min, 最大为1439min;<br>-1表示加载全部数据</p></body></html>"));

	m_pchkAllCheck = new QCheckBox("AllCheck", m_pgrpCtrl);
	m_pchkAllCheck->setChecked(true);

	m_pchkShowCheck = new QCheckBox("ShowCheck", m_pgrpCtrl);

	m_pcmbSelChannel = new QComboBox(m_pgrpCtrl);
	m_pcmbSelChannel->addItem(tr("CH 1-32"));
	m_pcmbSelChannel->addItem(tr("CH 33-64"));
	m_pcmbSelChannel->addItem(QStringLiteral("CH 65-96"));
	m_pcmbSelChannel->addItem(QStringLiteral("CH 97-128"));
	m_pcmbSelChannel->addItem(QStringLiteral("CH 129-160"));
	m_pcmbSelChannel->addItem(QStringLiteral("CH 161-192"));
	m_pcmbSelChannel->addItem(QStringLiteral("CH 193-224"));
	m_pcmbSelChannel->addItem(QStringLiteral("CH 225-256"));
	m_pcmbSelChannel->addItem(tr("AllChannel"));
	m_pcmbSelChannel->addItem(tr("First 128"));
	m_pcmbSelChannel->addItem(tr("Last 128"));
	m_pcmbSelChannel->setCurrentIndex(8);

	m_pdspFilterMin = new QDoubleSpinBox(m_pgrpCtrl);
	//m_pdspFilterMin->setSuffix(" pF");
	m_pdspFilterMin->setDecimals(2);
	m_pdspFilterMin->setMinimum(0.0);
	m_pdspFilterMin->setMaximum(1000.0);
	m_pdspFilterMin->setValue(10.0);

	m_pdspFilterMax = new QDoubleSpinBox(m_pgrpCtrl);
	//m_pdspFilterMax->setSuffix(" pF");
	m_pdspFilterMax->setDecimals(2);
	m_pdspFilterMax->setMinimum(0.0);
	m_pdspFilterMax->setMaximum(1000.0);
	m_pdspFilterMax->setValue(150.0);

	m_pbtnHistoryFilter = new QPushButton("Filter", m_pgrpCtrl);

	m_plabHistoryFilter = new QLabel("--", m_pgrpCtrl);
	m_plabHistoryFilter->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>显示符合筛选条件的通道个数</p></body></html>"));

	m_plistHistoryShow = new QListWidget(m_pgrpCtrl);
	m_vetHistoryListItem.resize(CHANNEL_NUM);
	for (int i = 0; i < CHANNEL_NUM; ++i)
	{
		auto widitem = new QListWidgetItem(m_plistHistoryShow);
		//auto listitem = new HistoryListItem(i, m_plistHistoryShow);
		m_vetHistoryListItem[i] = std::make_shared<HistoryListItem>(i, m_plistHistoryShow);

		m_plistHistoryShow->addItem(widitem);
		m_plistHistoryShow->setItemWidget(widitem, m_vetHistoryListItem[i].get());

		connect(m_vetHistoryListItem[i].get(), &HistoryListItem::checkShowChangedSignal, this, &HistoryDataView::checkShowChangedSlot);
	}


	//QHBoxLayout* horlayout5 = new QHBoxLayout();
	//horlayout5->addWidget(m_pchkAllCheck);
	//horlayout5->addWidget(m_pchkShowCheck);

	QGridLayout* gdlayout5 = new QGridLayout();
	gdlayout5->addWidget(m_pchkAllCheck, 0, 0);
	gdlayout5->addWidget(m_pchkShowCheck, 0, 1);
	gdlayout5->addWidget(m_pcmbSelChannel, 1, 0);
	gdlayout5->addWidget(m_pispTimeNode, 1, 1);
	gdlayout5->addWidget(m_pdspFilterMin, 2, 0);
	gdlayout5->addWidget(m_pdspFilterMax, 2, 1);
	gdlayout5->addWidget(m_pbtnHistoryFilter, 3, 0);
	gdlayout5->addWidget(m_plabHistoryFilter, 3, 1, Qt::AlignCenter);

	QVBoxLayout* verlayout5 = new QVBoxLayout();
	//verlayout5->addLayout(horlayout5);
	//verlayout5->addWidget(m_pcmbSelChannel);
	verlayout5->addLayout(gdlayout5);
	verlayout5->addWidget(m_plistHistoryShow);

	m_pgrpCtrl->setLayout(verlayout5);


	QHBoxLayout* horlayout9 = new QHBoxLayout();
	horlayout9->addWidget(m_pgrpPlot);
	horlayout9->addWidget(m_pgrpCtrl);

	QVBoxLayout* verlayout9 = new QVBoxLayout();
	verlayout9->addLayout(horlayout9);

	setLayout(verlayout9);


	connect(m_pchkAllCheck, &QCheckBox::stateChanged, this, &HistoryDataView::OnCheckAllChecked);
	connect(m_pchkShowCheck, &QCheckBox::stateChanged, this, &HistoryDataView::OnCheckShowChecked);
	connect(m_pcmbSelChannel, SIGNAL(activated(int)), this, SLOT(OnPlotPageViewChange(int)));
	connect(m_pbtnHistoryFilter, &QPushButton::clicked, this, &HistoryDataView::OnClickFilterValue);
	connect(m_pbtnLoadFile, &QPushButton::clicked, this, &HistoryDataView::OnClickLoadFile);
	connect(m_pbtnReloadFile, &QPushButton::clicked, this, &HistoryDataView::OnClickReloadFile);
	connect(m_pbtnResetData, &QPushButton::clicked, this, &HistoryDataView::OnClickResetData);

	return false;
}

void HistoryDataView::GetActiveChannelIndex(int curIndex)
{
	if (curIndex >= 0 && curIndex <= 7)
	{
		m_iStartIndex = 32 * curIndex;
		m_iEndIndex = 32 + m_iStartIndex;
	}
	else if (curIndex == 8)
	{
		m_iStartIndex = 0;
		m_iEndIndex = CHANNEL_NUM;
	}
	else if (curIndex == 9)
	{
		m_iStartIndex = 0;
		m_iEndIndex = 128;
	}
	else if (curIndex == 10)
	{
		m_iStartIndex = 128;
		m_iEndIndex = 256;
	}
	else
	{
		m_iStartIndex = 0;
		m_iEndIndex = CHANNEL_NUM;
	}
}

void HistoryDataView::DoLoadDataFile(const QString& fileName)
{
	for (int i = 0; i < CHANNEL_NUM; ++i)
	{
		m_pHistoryPlot->graph(i)->data()->clear();
	}
	m_pHistoryPlot->replot(QCustomPlot::rpQueuedReplot);

	QFileInfo fileInfo(fileName);
	QString fileBaseName = fileInfo.baseName();
	m_peditFileName->setText(fileName);

	std::string strFileName = fileName.toStdString();
	FILE* pf = fopen(strFileName.c_str(), "r");
	if (pf != nullptr)
	{
		::fseek(pf, 0L, SEEK_END);
		int len = ::ftell(pf) + 1;

		char* pbuf = new char[len];
		memset(pbuf, 0, len);

		::fseek(pf, 0L, SEEK_SET);
		int buflen = ::fread(pbuf, 1, len, pf);

		std::vector<std::vector<std::string>> vetvetdt;
		std::vector<std::string> vetrow;
		UsbProtoMsg::SplitString(pbuf, "\n", vetrow);

		delete[] pbuf;
		pbuf = nullptr;
		fclose(pf);
		pf = nullptr;

		if (vetrow.size() > 0)
		{
			for (const auto& ite : vetrow)
			{
				std::vector<std::string> vetdt;
				std::string strdt = ite.substr(0, ite.size() - 1);//
				UsbProtoMsg::SplitString(strdt.c_str(), ",", vetdt);
				vetvetdt.emplace_back(vetdt);
			}

			double temp = 0;
			int timenode = m_pispTimeNode->value() * 60;
			bool dayloop = false;

			m_vetHistoryValue.resize(CHANNEL_NUM);
			for (int i = 1, row = vetvetdt.size(); i < row; ++i)
			{
				std::string dttm = vetvetdt[i][0];
				std::vector<std::string> vetdttm;
				UsbProtoMsg::SplitString(dttm.c_str(), ":", vetdttm);
				if (vetdttm.size() < 3)
				{
					continue;
				}
				int hor = ::atoi(vetdttm[0].c_str());
				int min = ::atoi(vetdttm[1].c_str());
				int sec = ::atoi(vetdttm[2].c_str());
				double tmval = 3600 * hor + 60 * min + sec;
				if (i == 1)
				{
					m_dFirstTimeval = tmval;
					tmval = 0.0;
				}
				else
				{
					tmval = tmval - m_dFirstTimeval;
					if (tmval <= 0 || dayloop)
					{
						dayloop = true;
						tmval += (24 * 3600);
					}
				}
				bool rech = (m_pispTimeNode->value() != -1 && tmval >= timenode);
				bool fini = (row >= 2 && i == row - 1);
				for (int j = 1, col = vetvetdt[i].size(); j < col; ++j)
				{
					temp = ::atof(vetvetdt[i][j].c_str());
					int idx = j - 1;
					if (idx >= CHANNEL_NUM)
					{
						LOGW("Wrong History Data Loading....");
						break;
					}
					m_pHistoryPlot->graph(idx)->addData(tmval, temp);

					if (rech || fini)
					{
						m_vetHistoryValue[idx] = temp;
						m_vetHistoryListItem[idx]->SetValue(QString::number(temp));
					}
				}
				if (rech)
				{
					break;
				}
			}

			OnClickResetData();
		}
	}
	else
	{
		LOGE("Can't Open Datafile={}", strFileName.c_str());
	}
}

void HistoryDataView::OnCheckAllChecked(bool chk)
{
	for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
	{
		if (!m_plistHistoryShow->item(i)->isHidden())
		{
			m_vetHistoryListItem[i]->SetChecked(chk);
		}
	}
}

void HistoryDataView::OnCheckShowChecked(bool chk)
{
	if (chk)
	{
		int cnt = 0;
		for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
		{
			bool chk2 = m_vetHistoryListItem[i]->GetChecked();
			m_plistHistoryShow->item(i)->setHidden(!chk2);

			if (chk2)
			{
				++cnt;
			}
		}
		m_plabHistoryFilter->setText(QString::number(cnt));
	}
	else
	{
		for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
		{
			m_plistHistoryShow->item(i)->setHidden(false);
		}
	}
}

void HistoryDataView::OnPlotPageViewChange(int index)
{
	GetActiveChannelIndex(index);
	bool onlyShow = m_pchkShowCheck->isChecked();
	int cnt = 0;
	for (int i = 0; i < CHANNEL_NUM; i++)
	{
		if (i >= m_iStartIndex && i < m_iEndIndex)
		{
			bool chk = m_vetHistoryListItem[i]->GetChecked();
			//vetChecked[i] = chk;
			bool hide = onlyShow ? (!chk) : false;
			m_plistHistoryShow->item(i)->setHidden(hide);
			if (!hide)
			{
				++cnt;
			}
			checkShowChangedSlot(i, chk);
		}
		else
		{
			m_plistHistoryShow->item(i)->setHidden(true);
			checkShowChangedSlot(i, false);
		}
	}

	m_plabHistoryFilter->setText(QString::number(cnt));
}

void HistoryDataView::OnClickFilterValue(void)
{
	if (m_vetHistoryValue.size() < CHANNEL_NUM)
	{
		return;
	}
	double minval = m_pdspFilterMin->value();
	double maxval = m_pdspFilterMax->value();
	int cnt = 0;
	for (int i = 0; i < CHANNEL_NUM; ++i)
	{
		const auto& val = m_vetHistoryValue[i];
		if (val >= minval && val <= maxval)
		{
			m_vetHistoryListItem[i]->SetChecked(true);
			bool hid = (i < m_iStartIndex || i >= m_iEndIndex);
			m_plistHistoryShow->item(i)->setHidden(hid);
			++cnt;
		}
		else
		{
			m_vetHistoryListItem[i]->SetChecked(false);
			m_plistHistoryShow->item(i)->setHidden(true);
		}
	}
	m_plabHistoryFilter->setText(QString::number(cnt));
}

void HistoryDataView::OnClickLoadFile(void)
{
	QString qsfile = QFileDialog::getOpenFileName(this,
		tr("Open Data File"),
		".",
		"csv files(*.csv)",
		0);

	if (!qsfile.isNull())
	{
		DoLoadDataFile(qsfile);
	}
}

void HistoryDataView::OnClickReloadFile(void)
{
	QString qsfile = m_peditFileName->text();
	if (!qsfile.isNull())
	{
		DoLoadDataFile(qsfile);
	}
	else
	{
		LOGW("Datafile Name is Empty!");
	}
}

void HistoryDataView::OnClickResetData(void)
{
	std::thread thrd([this]()
		{
			m_pHistoryPlot->xAxis->rescale();
			m_pHistoryPlot->yAxis->rescale();
			//m_graph->rescaleValueAxis(false, true);
			m_pHistoryPlot->replot(QCustomPlot::rpQueuedReplot);
			//m_pHistoryPlot->layer("histlayer1")->replot(true);
		});
	thrd.detach();
}

void HistoryDataView::OnPlotApply(void)
{
	double ymax = m_pdspYmax->value();
	double ymin = m_pdspYmin->value();
	double xtim = m_pdspPtnum->value();

	ConfigServer::GetInstance()->SetPlotYmax(ymax);
	ConfigServer::GetInstance()->SetPlotYmin(ymin);
	ConfigServer::GetInstance()->SetPlotXnum(xtim);

	m_pHistoryPlot->xAxis->setRange(0.0, xtim);
	m_pHistoryPlot->yAxis->setRange(ymin, ymax);
	//m_pHistoryPlot->xAxis->rescale();
	//m_pHistoryPlot->yAxis->rescale();

	m_pHistoryPlot->replot(QCustomPlot::rpQueuedReplot);
}

void HistoryDataView::MyMouseMoveEvent(QMouseEvent* event)
{
#if 0

	if (m_pHistoryPlot->graphCount() == 0)
		return;

	//获取鼠标坐标，相对父窗体坐标
	int x_pos = event->pos().x();
	int y_pos = event->pos().y();

	//鼠标坐标转化为CustomPlot内部坐标
	float x_val = m_pHistoryPlot->xAxis->pixelToCoord(x_pos);
	float y_val = m_pHistoryPlot->yAxis->pixelToCoord(y_pos);

	//通过坐标轴范围判断光标是否在点附近
	float x_begin = m_pHistoryPlot->xAxis->range().lower;
	float x_end = m_pHistoryPlot->xAxis->range().upper;
	float y_begin = m_pHistoryPlot->yAxis->range().lower;
	float y_end = m_pHistoryPlot->yAxis->range().upper;
	float x_tolerate = (x_end - x_begin) / 40;//光标与最近点距离在此范围内，便显示该最近点的值
	float y_tolerate = (y_end - y_begin) / 40;

	//判断有没有超出坐标轴范围
	if (x_val < x_begin || x_val > x_end)
		return;

	//通过x值查找离曲线最近的一个key值索引
	int index = 0;
	int index_left = m_pHistoryPlot->graph(0)->findBegin(x_val, true);//左边最近的一个key值索引
	int index_right = m_pHistoryPlot->graph(0)->findEnd(x_val, true);//右边
	float dif_left = fabs(m_pHistoryPlot->graph(0)->data()->at(index_left)->key - x_val);
	float dif_right = fabs(m_pHistoryPlot->graph(0)->data()->at(index_right)->key - x_val);
	if (dif_left < dif_right)
		index = index_left;
	else
		index = index_right;

	x_val = m_pHistoryPlot->graph(0)->data()->at(index)->key;//通过得到的索引获取key值

	int graphIndex = 0;//curve index closest to the cursor
	float dx = fabs(x_val - m_pHistoryPlot->xAxis->pixelToCoord(x_pos));
	float dy = fabs(m_pHistoryPlot->graph(0)->data()->at(index)->value - y_val);

	//通过遍历每条曲线在index处的value值，得到离光标点最近的value及对应曲线索引
	for (int i = 0; i < m_pHistoryPlot->xAxis->graphs().count(); i++)
	{
		if (fabs(m_pHistoryPlot->graph(i)->data()->at(index)->value - y_val) < dy)
		{
			dy = fabs(m_pHistoryPlot->graph(i)->data()->at(index)->value - y_val);
			graphIndex = i;
		}
	}

	y_val = m_pHistoryPlot->graph(graphIndex)->data()->at(index)->value;
	//QString strToolTip = QString("CH%1:(%2, %3)").arg(graphIndex + 1).arg(x_val).arg(y_val);
	int hor = x_val / 3600;
	int tmp = (int)x_val % 3600;
	int min = tmp / 60;
	int sec = tmp % 60;
	char arx[32] = { 0 };
	sprintf(arx, "%02d:%02d:%02d", hor, min, sec);
	QString strToolTip = QString("CH%1 \n(%2, %3)").arg(graphIndex + 1).arg(arx).arg(y_val);

	//判断光标点与最近点的距离是否在设定范围内
	if (dy < y_tolerate && dx < x_tolerate)
		QToolTip::showText(cursor().pos(), strToolTip, m_pHistoryPlot);

#else

	if (m_pHistoryPlot->graphCount() == 0)
	{
		return;
	}

	//获取鼠标坐标，相对父窗体坐标
	int x_pos = event->pos().x();
	int y_pos = event->pos().y();

	//鼠标坐标转化为CustomPlot内部坐标
	float x_val = m_pHistoryPlot->xAxis->pixelToCoord(x_pos);
	float y_val = m_pHistoryPlot->yAxis->pixelToCoord(y_pos);

	//通过坐标轴范围判断光标是否在点附近
	float x_begin = m_pHistoryPlot->xAxis->range().lower;
	float x_end = m_pHistoryPlot->xAxis->range().upper;
	float y_begin = m_pHistoryPlot->yAxis->range().lower;
	float y_end = m_pHistoryPlot->yAxis->range().upper;
	float x_tolerate = (x_end - x_begin) / 40;//光标与最近点距离在此范围内，便显示该最近点的值
	float y_tolerate = (y_end - y_begin) / 40;

	//判断有没有超出坐标轴范围
	if (x_val < x_begin || x_val > x_end)
	{
		return;
	}

	//通过x值查找离曲线最近的一个key值索引
	int index_left = m_pHistoryPlot->graph(0)->findBegin(x_val, true);//左边最近的一个key值索引
	int index_right = m_pHistoryPlot->graph(0)->findEnd(x_val, true);//右边
	float dif_left = fabs(m_pHistoryPlot->graph(0)->data()->at(index_left)->key - x_val);
	float dif_right = fabs(m_pHistoryPlot->graph(0)->data()->at(index_right)->key - x_val);
	int index = ((dif_left < dif_right) ? index_left : index_right);
	//if (dif_left < dif_right)
	//	index = index_left;
	//else
	//	index = index_right;

	//float diff = fabs(m_pHistoryPlot->graph(0)->data()->at(index_right)->key - m_pHistoryPlot->graph(0)->data()->at(index_left)->key);
	//if (x_val >= m_pHistoryPlot->graph(0)->data()->at(index_left)->key + diff / 2.0)
	//{
	//	index = index_right;
	//}
	//else
	//{
	//	index = index_left;
	//}

	double x_posval = m_pHistoryPlot->graph(0)->data()->at(index)->key;//通过得到的索引获取key值
	double y_posval = m_pHistoryPlot->graph(0)->data()->at(index)->value;//通过得到的索引获取value值

	float dx = fabs(x_posval - x_val);
	float dy = fabs(y_posval - y_val);

	int graphIndex = -1;//curve index closest to the cursor
	//通过遍历每条曲线在index处的value值，得到离光标点最近的value及对应曲线索引
	for (int i = 0, n = m_pHistoryPlot->xAxis->graphs().count(); i < n; i++)
	{
		y_posval = fabs(m_pHistoryPlot->graph(i)->data()->at(index)->value - y_val);
		if (y_posval < dy && m_pHistoryPlot->graph(i)->visible())
		{
			dy = y_posval;
			graphIndex = i;
		}
	}

	//判断光标点与最近点的距离是否在设定范围内
	if (graphIndex >= 0 && dy <= y_tolerate && dx <= x_tolerate)
	{
		y_posval = m_pHistoryPlot->graph(graphIndex)->data()->at(index)->value;
		//QString strToolTip = QString("CH%1:(%2, %3)").arg(graphIndex + 1).arg(x_val).arg(y_val);
		//auto qstr = QDateTime::fromSecsSinceEpoch(x_posval).toString("hh:mm:ss");
		int hor = x_posval / 3600;
		int tmp = (int)x_posval % 3600;
		int min = tmp / 60;
		int sec = tmp % 60;
		char arx[32] = { 0 };
		sprintf(arx, "%02d:%02d:%02d", hor, min, sec);
		QString strToolTip = QString("CH%1 \nx=%2\ny=%3").arg(graphIndex + 1).arg(arx).arg(y_posval);

		QToolTip::showText(cursor().pos(), strToolTip, m_pHistoryPlot);
	}

#endif
}

void HistoryDataView::checkShowChangedSlot(int chan, bool chk)
{
	m_pHistoryPlot->graph(chan)->setVisible(chk);
	m_pHistoryPlot->replot(QCustomPlot::rpQueuedReplot);
}
