#include "CapListItem.h"
#include <qlayout.h>


CapListItem::CapListItem(int chan, QWidget *parent) 
	: QWidget(parent)
	, m_iChannel(chan)
{
	InitCtrl();
}

bool CapListItem::InitCtrl(void)
{
	m_plabChannel = new QLabel(QString("CH%1").arg(m_iChannel + 1), this);

	m_plabCapValue = new QLabel("", this);
	m_plabCapValue->setMinimumWidth(70);
	m_plabCapValue->setFrameShape(QFrame::StyledPanel);

	//m_plabCapUnit = new QLabel("pF", this);

	QHBoxLayout* horlayout = new QHBoxLayout();
	horlayout->setMargin(0);
	horlayout->addWidget(m_plabChannel);
	horlayout->addWidget(m_plabCapValue);
	//horlayout->addWidget(m_plabCapUnit);
	horlayout->addSpacing(5);

	setLayout(horlayout);

	return false;
}
