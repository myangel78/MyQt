#pragma once

#include <QToolButton>
#include <QSplitter>
#include "Global.h"


class CustomSplitterHandle : public QSplitterHandle
{
    Q_OBJECT
public:

    CustomSplitterHandle(Qt::Orientation orientation, SPLITTER_HIDDEN_DIRECTION_ENUM shd, QSplitter *parent);
    ~CustomSplitterHandle();
    void SetButtonChecked(bool checked);
    void ToggleShowHideClick(void){m_pToolButton->toggle();}

signals:
    void OnHideWidgetSig(bool bIsHide);

public slots:
    void OnButtonclicked(bool checked);

private:
    SPLITTER_HIDDEN_DIRECTION_ENUM m_HideDirection;
    QToolButton *m_pToolButton;
};


class CustomSplitter : public QSplitter
{
    Q_OBJECT
public:
    CustomSplitter(Qt::Orientation orientation, SPLITTER_HIDDEN_DIRECTION_ENUM shd, QWidget* parent = Q_NULLPTR);
    void SetHiddenDirection(SPLITTER_HIDDEN_DIRECTION_ENUM shd);

    void ToggleShowHideClick(void){ m_pSplitterHandle->ToggleShowHideClick();}
    bool GetWgtIsVisible(void) const{ return !m_bIsHideWdgt;}

protected:
    QSplitterHandle *createHandle();

public slots:
    void OnHideWidgetSlot(bool bIsHide);
    void OnSplitterMovedSlot(int pos, int index);

private:
    CustomSplitterHandle *m_pSplitterHandle;
    QByteArray m_oOldState;
    SPLITTER_HIDDEN_DIRECTION_ENUM m_HideDirection;

    bool m_bIsHideWdgt;
};
