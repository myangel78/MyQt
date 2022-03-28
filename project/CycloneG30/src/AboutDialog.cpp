#include "AboutDialog.h"
#include <qlayout.h>
#include <qfile.h>
#include <qtextstream.h>
#include "mainversion.h"


AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
	setFixedSize(400, 400); // 不能伸缩的对话框
	InitCtrl();
}

//AboutDialog::~AboutDialog()
//{
//	delete m_plabLogoImage;
//	delete m_plabAboutInfo;
//	delete m_plabLicence;
//	delete m_pgrpLicence;
//	delete m_peditReleaseLog;
//}

bool AboutDialog::InitCtrl(void)
{
	m_plabLogoImage = new QLabel(this);
    QPixmap pix(":/img/img/cycloneG30.png");
	pix = pix.scaledToWidth(110, Qt::SmoothTransformation);
	m_plabLogoImage->setPixmap(pix);

	m_plabAboutInfo = new QLabel(this);
	m_plabAboutInfo->setText(
        "<b>CycloneG30</b><br>"
		"Version:" SOFTWARE_VERSION " (" BUILD_VERSION ")<br>"
		"Theme :		BGI Research<br>"
		"Home Page:		<a href=\"https://www.genomics.cn\">https://www.genomics.cn</a><br>"
		"Build Date:	" __DATE__ "<br>" // 编译时间
		COPYRIGHT
	);

	m_plabAboutInfo->setOpenExternalLinks(true); // 允许访问链接


	m_pgrpLicence = new QGroupBox("BGI General Public License", this);
	m_plabLicence = new QLabel("The program is provided gemomics seqence.\n....\n....", m_pgrpLicence);
	m_plabLicence->setWordWrap(true);
	m_plabLicence->setText("\tFounded in 1999, BGI is one of the world's leading life science and genomics organizations. BGI's mission is to use genomics to benefit mankind and to be a leader in the era of life sciences. BGI is committed to applying its genetic and technological achievements to real world settings in order to realize the dream of trans-omics for a better life.");
	QVBoxLayout* verlayout1 = new QVBoxLayout();
	verlayout1->addWidget(m_plabLicence);
	m_pgrpLicence->setLayout(verlayout1);


	m_peditReleaseLog = new QTextEdit(this);
	m_peditReleaseLog->setReadOnly(true);


	QHBoxLayout* horlayout6 = new QHBoxLayout();
	horlayout6->addWidget(m_plabLogoImage);
	horlayout6->addWidget(m_plabAboutInfo);

	QVBoxLayout* verlayout6 = new QVBoxLayout();
	verlayout6->addLayout(horlayout6);
	verlayout6->addWidget(m_pgrpLicence);
	verlayout6->addWidget(m_peditReleaseLog);

	setLayout(verlayout6);


	LoadReleaseInfo();

	return false;
}

bool AboutDialog::LoadReleaseInfo(void)
{
    QFile file(":/info/info/ReleaseInfo.txt");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}
	else
	{
		if (!file.isReadable())
		{
			return false;
		}
		else
		{
			QTextStream textStream(&file);
			while (!textStream.atEnd())
			{
				m_peditReleaseLog->setPlainText(textStream.readAll());
			}
			m_peditReleaseLog->show();
			file.close();
		}
	}
	return true;
}
