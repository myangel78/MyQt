#include "CustomSplitter.h"
#include <QCheckBox>
#include <QVBoxLayout>
#include <QToolButton>
#include <QVariant>
#include <QDebug>
#include <QMouseEvent>

CustomSplitter::CustomSplitter(Qt::Orientation orientation, SPLITTER_HIDDEN_DIRECTION_ENUM shd, QWidget *parent)
    : QSplitter(orientation, parent)
    , m_HideDirection(shd)
{
    setHandleWidth(8);
    m_bIsHideWdgt = false;
    connect(this, SIGNAL(splitterMoved(int, int)), this, SLOT(OnSplitterMovedSlot(int, int)));
}

void CustomSplitter::SetHiddenDirection(SPLITTER_HIDDEN_DIRECTION_ENUM shd)
{
    m_HideDirection = shd;
}

QSplitterHandle *CustomSplitter::createHandle()
{
    m_pSplitterHandle = new CustomSplitterHandle(orientation(), m_HideDirection, this);
    connect(m_pSplitterHandle, SIGNAL(OnHideWidgetSig(bool)), this, SLOT(OnHideWidgetSlot(bool)));
    return m_pSplitterHandle;
}

void CustomSplitter::OnHideWidgetSlot(bool bIsHide)
{
    m_bIsHideWdgt = bIsHide;
    if (bIsHide)
    {
       m_oOldState = saveState();
    }
    else
    {
        if (!m_oOldState.isEmpty())
        {
            restoreState(m_oOldState);
            return;
        }
    }

    QList<int> widthOfAllWidgt(sizes());
    switch (m_HideDirection)
    {
    case SPLITTER_HIDE_DIRECT_LEFT:
    case SPLITTER_HIDE_DIRECT_TOP:
    {
        if (bIsHide)
        {
            widthOfAllWidgt[1] += widthOfAllWidgt[0];
            widthOfAllWidgt[0] = 0;
        }
        else
        {
            widthOfAllWidgt[0] = 40;
            widthOfAllWidgt[1] = widthOfAllWidgt[1] - 40;
        }
        break;
    }
    case SPLITTER_HIDE_DIRECT_RIGHT:
    case SPLITTER_HIDE_DIRECT_BOTTOM:
    {
        if (bIsHide)
        {
            widthOfAllWidgt[widthOfAllWidgt.count() - 2] += widthOfAllWidgt[widthOfAllWidgt.count() - 1];
            widthOfAllWidgt[widthOfAllWidgt.count() - 1] = 0;
        }
        else
        {
            widthOfAllWidgt[widthOfAllWidgt.count() - 1] = 40;
            widthOfAllWidgt[widthOfAllWidgt.count() - 2] = widthOfAllWidgt[widthOfAllWidgt.count() - 2] - 40;
        }
        break;
    }
    default:
        break;
    }

    this->setSizes(widthOfAllWidgt);
}

void CustomSplitter::OnSplitterMovedSlot(int pos, int index)
{
    switch (m_HideDirection)
    {
    case SPLITTER_HIDE_DIRECT_LEFT:
    case SPLITTER_HIDE_DIRECT_TOP:
    {
        m_pSplitterHandle->SetButtonChecked(pos == 0 ? true : false);
        break;
    }
    case SPLITTER_HIDE_DIRECT_RIGHT:
    case SPLITTER_HIDE_DIRECT_BOTTOM:
    {
        m_pSplitterHandle->SetButtonChecked((pos == this->width() - m_pSplitterHandle->width()) ? true : false);
        break;
    }
    default:
        break;
    }
}

CustomSplitterHandle::CustomSplitterHandle(Qt::Orientation orientation, SPLITTER_HIDDEN_DIRECTION_ENUM shd, QSplitter *parent)
    : QSplitterHandle(orientation, parent)
    , m_HideDirection(shd)
{
    this->setMouseTracking(false);
    m_pToolButton = new QToolButton(this);

    m_pToolButton->setCursor(Qt::ArrowCursor);
    m_pToolButton->setCheckable(true);
    connect(m_pToolButton, SIGNAL(toggled(bool)), this, SLOT(OnButtonclicked(bool)));
    m_pToolButton->setProperty("HiddenDirection", QVariant((int)(m_HideDirection)));

    this->setStyleSheet("\
    QToolButton\
    {\
        border: none;\
    }\
    QToolButton[HiddenDirection=\"1\"]\
    {\
        background-image: url(:/img/img/splitterLeft.png);\
    }\
    QToolButton[HiddenDirection=\"1\"]::checked\
    {\
        background-image: url(:/img/img/splitterRight.png);\
    }\
    QToolButton[HiddenDirection=\"2\"]\
    {\
        background-image: url(:/img/img/splitterRight.png);\
    }\
    QToolButton[HiddenDirection=\"2\"]::checked\
    {\
        background-image: url(:/img/img/splitterLeft.png);\
    }\
    QToolButton[HiddenDirection=\"3\"]\
    {\
        background-image: url(:/img/img/splitterTop.png);\
    }\
    QToolButton[HiddenDirection=\"3\"]::checked\
    {\
        background-image: url(:/img/img/splitterBottom.png);\
    }\
    QToolButton[HiddenDirection=\"4\"]\
    {\
        background-image: url(:/img/img/splitterBottom.png);\
    }\
    QToolButton[HiddenDirection=\"4\"]:checked\
    {\
        background-image: url(:/img/img/splitterTop.png);\
    }\
    ");

    QLayout *pLayout;
    if (orientation == Qt::Horizontal)
    {
        m_pToolButton->setFixedSize(8, 68);
        pLayout = new QVBoxLayout;
    }
    else
    {
        m_pToolButton->setFixedSize(68, 8);
        pLayout = new QHBoxLayout;
    }
    pLayout->setContentsMargins(0,0,0,0);
    pLayout->addWidget(m_pToolButton);

    setLayout(pLayout);
}

CustomSplitterHandle::~CustomSplitterHandle()
{

}

void CustomSplitterHandle::SetButtonChecked(bool checked)
{
    m_pToolButton->setChecked(checked);
}

void CustomSplitterHandle::OnButtonclicked(bool checked)
{
    emit OnHideWidgetSig(checked);
}
