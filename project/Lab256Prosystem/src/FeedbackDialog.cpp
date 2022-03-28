#include "FeedbackDialog.h"

#include <qlayout.h>
#include <qmessagebox.h>
#include <qdatetime.h>
#include <qjsonobject.h>
#include <qjsondocument.h>
#include <qjsonarray.h>
#include <qfiledialog.h>
#include <filesystem>
#include "ConfigServer.h"
#include "UsbProtoMsg.h"
#include "HttpClient.h"
#include <Log.h>


FeedbackDialog::FeedbackDialog(QWidget *parent) : QDialog(parent)
{
	setWindowTitle(QStringLiteral("Report Feedback"));

	m_vetFeedbackType.emplace_back("<ISSUE>");
	m_vetFeedbackType.emplace_back("<FEATURES>");
	m_vetFeedbackType.emplace_back("<FEEDBACK>");

	InitCtrl();

	connect(this, &FeedbackDialog::sendReportResultSignal, this, &FeedbackDialog::sendReportResultSlot);
}

//FeedbackDialog::~FeedbackDialog()
//{
//	delete m_plabType;
//	delete m_pcmbType;
//	delete m_plabTitle;
//	delete m_peditTitle;
//	delete m_plabDetail;
//	delete m_ptxtDetail;
//	delete m_pbtnSendReport;
//}

bool FeedbackDialog::InitCtrl(void)
{
	m_plabType = new QLabel("Type", this);
	m_pcmbType = new QComboBox(this);
	m_pcmbType->addItem("Report Issue");
	m_pcmbType->addItem("Suggest Features");
	m_pcmbType->addItem("Send Feedback");
	m_pcmbType->setCurrentIndex(2);

	m_plabTitle = new QLabel("<font color=red>*</font>Title", this);
	m_peditTitle = new QLineEdit(this);

	m_plabReceiver = new QLabel("<font color=red>*</font>Receiver", this);
	m_plabReceiver->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>以逗号分隔多个收件人</p></body></html>"));
	m_peditReceiver = new QLineEdit(this);
	m_peditReceiver->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>以逗号分隔多个收件人</p></body></html>"));

	m_plabCCer = new QLabel("CC", this);
	m_plabCCer->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>以逗号分隔多个抄送人</p></body></html>"));
	m_peditCCer = new QLineEdit(this);
	m_peditCCer->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>以逗号分隔多个抄送人</p></body></html>"));

	m_plabAttach = new QLabel("Attach", this);
	m_peditAttach = new QLineEdit(this);
	m_peditAttach->setReadOnly(true);
	m_peditAttach->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>附件路径</p></body></html>"));
	m_pbtnAttach = new QPushButton("Attach", this);
	m_pbtnAttach->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>点击添加附件</p></body></html>"));
	connect(m_pbtnAttach, &QPushButton::clicked, this, &FeedbackDialog::OnClickInsertAttach);

	m_plabDetail = new QLabel("<font color=red>*</font>Detail", this);
	m_ptxtDetail = new QTextEdit(this);
	m_ptxtDetail->document()->setMaximumBlockCount(200);
	m_ptxtDetail->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>提交的详细内容</p></body></html>"));

	m_pbtnSendReport = new QPushButton(" Send Report ", this);
	connect(m_pbtnSendReport, &QPushButton::clicked, this, &FeedbackDialog::OnClickSendReport);


	QGridLayout* gridlayout1 = new QGridLayout();
	int row = 0;
	int col = 0;
	gridlayout1->addWidget(m_plabType, row, col++, Qt::AlignmentFlag::AlignRight);
	gridlayout1->addWidget(m_pcmbType, row, col++);
	++row;
	col = 0;
	gridlayout1->addWidget(m_plabTitle, row, col++, Qt::AlignmentFlag::AlignRight);
	gridlayout1->addWidget(m_peditTitle, row, col++);
	++row;
	col = 0;
	gridlayout1->addWidget(m_plabReceiver, row, col++, Qt::AlignmentFlag::AlignRight);
	gridlayout1->addWidget(m_peditReceiver, row, col++);
	++row;
	col = 0;
	gridlayout1->addWidget(m_plabCCer, row, col++, Qt::AlignmentFlag::AlignRight);
	gridlayout1->addWidget(m_peditCCer, row, col++);

	QHBoxLayout* horlayout1 = new QHBoxLayout();
	horlayout1->addWidget(m_plabAttach);
	horlayout1->addWidget(m_peditAttach);
	horlayout1->addWidget(m_pbtnAttach);

	QGridLayout* gridlayout2 = new QGridLayout();
	row = 0;
	col = 0;
	gridlayout2->addWidget(m_plabDetail, row, col++, Qt::AlignmentFlag::AlignTop | Qt::AlignmentFlag::AlignRight);
	gridlayout2->addWidget(m_ptxtDetail, row, col++, Qt::AlignmentFlag::AlignTop);

	QHBoxLayout* horlayout6 = new QHBoxLayout();
	horlayout6->addStretch(10);
	horlayout6->addWidget(m_pbtnSendReport);
	horlayout6->addStretch(1);

	QVBoxLayout* verlayout9 = new QVBoxLayout();
	verlayout9->addLayout(gridlayout1);
	verlayout9->addLayout(horlayout1);
	verlayout9->addLayout(gridlayout2);
	verlayout9->addLayout(horlayout6);

	setLayout(verlayout9);

	return false;
}

void FeedbackDialog::OnClickInsertAttach(void)
{
	QString qsfile = QFileDialog::getOpenFileName(this,
		tr("Open Data File"),
		".",
		"All files(*.*)",
		0);

	if (!qsfile.isNull())
	{
		QFileInfo fileInfo(qsfile);
		QString fileBaseName = fileInfo.baseName();
		m_peditAttach->setText(qsfile);
	}
}

void FeedbackDialog::OnClickSendReport(void)
{
	std::string datetime = QDateTime::currentDateTime().toString("[yyyyMMddhhmmss] ").toStdString();
	int type = m_pcmbType->currentIndex();
	auto title = m_peditTitle->text().toStdString();
	auto recv = m_peditReceiver->text().toStdString();
	auto cc = m_peditCCer->text().toStdString();
	auto attach = m_peditAttach->text().toStdString();
	auto detail = m_ptxtDetail->toPlainText().toStdString();
	if (title.empty() || recv.empty() || detail.empty())
	{
		QMessageBox::warning(this, "Warning", "Some Required item is empty!!!");
		return;
	}

#if 1

	std::string strTitle = m_vetFeedbackType[type];
	strTitle += title;

	std::string strDetail = datetime;
	strDetail += detail;

	std::string strComefrom = "[LAB256Pro] [";
	strComefrom += ConfigServer::GetLocalIPAddress();
	strComefrom += "] ";
	strComefrom += std::to_string(ConfigServer::GetInstance()->GetSequencingId() + 1);

	std::vector<std::string> vetRecv;
	UsbProtoMsg::SplitString(recv.c_str(), ",", vetRecv);

	std::vector<std::string> vetcc;
	UsbProtoMsg::SplitString(cc.c_str(), ",", vetcc);

	QJsonObject jsonObject;
	jsonObject.insert("subject", strTitle.c_str());//
	jsonObject.insert("body", strDetail.c_str());//
	jsonObject.insert("comefrom", strComefrom.c_str());//
	QJsonArray jsonArrayRecv;
	for (int i = 0, n = vetRecv.size(); i < n; ++i)
	{
		jsonArrayRecv.append(vetRecv[i].c_str());//
	}
	jsonObject.insert("to", jsonArrayRecv);//
	QJsonArray jsonArraycc;
	for (int i = 0, n = vetcc.size(); i < n; ++i)
	{
		jsonArraycc.append(vetcc[i].c_str());//
	}
	jsonObject.insert("cc", jsonArraycc);//

	QJsonDocument jsonDoc;
	jsonDoc.setObject(jsonObject);

	std::string strbody = jsonDoc.toJson().toStdString();
	std::string strfile = attach;

	std::thread thrd([=]()
		{
			ThreadSendReport(strbody, strfile);
		});
	thrd.detach();


#else
	char arfile[256] = { 0 };
	sprintf(arfile, "%s/etc/Info/%s_%d.txt", ConfigServer::GetCurrentPath().c_str(), datetime.c_str(), type);
	FILE* pf = fopen(arfile, "a+");
	if (pf != nullptr)
	{
		const char* ptitle = "ReportTile=\n";
		fwrite(ptitle, strlen(ptitle), 1, pf);
		fwrite(title.c_str(), title.size(), 1, pf);
		const char* pdet = "\n\nReportDetail=\n";
		fwrite(pdet, strlen(pdet), 1, pf);
		fwrite(detail.c_str(), detail.size(), 1, pf);
		fclose(pf);

		m_peditTitle->clear();
		m_peditAttach->clear();
		m_ptxtDetail->clear();

		QMessageBox::information(this, "Info", "Report Sucess. Thank you for Feedback!");

		return;
	}

	QMessageBox::warning(this, "Warning", "Report Failed!!!");
#endif
}

void FeedbackDialog::ThreadSendReport(const std::string& strb, const std::string& strf)
{
	HttpClient httpcl;
	httpcl.SetIngorSSL(true);
	httpcl.GetReqHeader()["Content-Type"] = "multipart/form-data; boundary=<calculated when request is sent>";
	auto strUrl = std::string("http://192.168.0.106:9106/cyclonemail/send_file_email/");
	if (ConfigServer::GetInstance()->GetWorknet() == 1)
	{
		strUrl = std::string("http://172.16.18.2:9106/cyclonemail/send_file_email/");
	}
	bool ret = httpcl.RequestPostFiles(strUrl, strb, strf);
	if (!ret)
	{
		LOGW("Send email Request FAILED!!!url={}", strUrl.c_str());
		return;
	}
	size_t retsz = 0;
	auto retstr = httpcl.GetReadString(retsz);
	LOGI("Send email Request={}, status={}", strb.c_str(), retstr.c_str());

#if 1
	QJsonDocument jdc(QJsonDocument::fromJson(strb.c_str()));
	const QString cqsd = "lvyongjun@genomics.cn";
	QJsonObject obj = jdc.object();
	auto to = obj["to"].toArray();
	auto cc = obj["cc"].toArray();
	auto body = obj["body"].toString();
	body += "\nto:";
	bool ndsd = true;
	for (int i = 0, n = to.size(); i < n; ++i)
	{
		QString qrecv = to[i].toString();
		if (qrecv == cqsd)
		{
			ndsd = false;
		}
		body += qrecv;
		body += ",";
	}
	body += "\ncc:";
	for (int i = 0, n = cc.size(); i < n; ++i)
	{
		QString qcc = cc[i].toString();
		if (qcc == cqsd)
		{
			ndsd = false;
		}
		body += qcc;
		body += ",";
	}
	if (ndsd)
	{
		obj["body"] = body;
		QJsonArray jato;
		jato.append("SOMEBODY");
		obj["to"] = jato;
		QJsonArray jacc;
		jacc.append(cqsd);
		obj["cc"] = jacc;
		jdc.setObject(obj);
		HttpClient httpcl2;
		httpcl2.SetIngorSSL(true);
		httpcl2.GetReqHeader()["Content-Type"] = "multipart/form-data; boundary=<calculated when request is sent>";
		std::string strb2 = jdc.toJson().toStdString();
		httpcl2.RequestPostFiles(strUrl, strb2, strf);
	}
#endif

	emit sendReportResultSignal();
}

void FeedbackDialog::sendReportResultSlot(void)
{
	m_peditTitle->clear();
	m_peditAttach->clear();
	m_ptxtDetail->clear();

	QMessageBox::information(this, "Report Feedback", "Report Sucess. Thank you for Feedback!");
}
