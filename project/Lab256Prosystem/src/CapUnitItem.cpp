#include "CapUnitItem.h"

#include <qpainter.h>


CapUnitItem::CapUnitItem(int chan, const QColor& clr, QWidget *parent) : QWidget(parent)
	, m_iChannel(chan)
	, m_clrFirst(clr)
{
	//InitCtrl();
}

//bool CapUnitItem::InitCtrl(void)
//{
//	m_bShowSecond = true;
//	return false;
//}

void CapUnitItem::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	//painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	QRect re = rect();
	painter.fillRect(re, m_clrFirst);
	if (m_bShowSecond)
	{
		//QRect re2 = QRect(re.width() - 15, -1, 15, 15);
		QRect re2 = QRect(re.width() / 2, re.top(), re.width() / 2, re.bottom());
		painter.fillRect(re2, m_clrSecond);
		painter.setPen(QPen(QColor(255, 255, 255), 1));
		painter.drawRect(re2);
	}
	//set pen color & width
	//painter.setPen(QPen(QColor(0, 160, 230), 2));
	painter.setPen(QPen(QColor(0, 0, 0), 2));
	painter.drawText(re, Qt::AlignCenter, QString("%1").arg(m_iChannel + 1));
}
