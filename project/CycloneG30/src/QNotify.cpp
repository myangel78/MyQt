/* Copyright 2013 Mohammed Nafees. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY MOHAMMED NAFEES ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL EELI REILIN OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed or implied, of Mohammed Nafees.
 */

// Self
#include "QNotify.h"

// Qt
#include <QPixmapCache>
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QTimer>
#include <QDebug>


static const std::vector<QColor> vetMsgColor =
{
    QColor{60,179,113}, QColor{255,0,0},QColor{100,149,237}
};


QNotify::QNotify( QWidget *parent ) :
    QWidget( parent )
{
//    Q_INIT_RESOURCE( res );
    QPixmapCache::insert( "success.png", QPixmap( ":/img/img/success.png" ).scaledToHeight( 50, Qt::SmoothTransformation ) );
    QPixmapCache::insert( "error.png", QPixmap( ":/img/img/error.png" ).scaledToHeight( 50, Qt::SmoothTransformation ) );
    QPixmapCache::insert( "warning.png", QPixmap( ":/img/img/warning.png" ).scaledToHeight( 50, Qt::SmoothTransformation ) );

    m_pixMapArray[0] = QPixmap(":/img/img/success.png").scaledToHeight(50, Qt::SmoothTransformation );
    m_pixMapArray[1] = QPixmap(":/img/img/error.png").scaledToHeight(50, Qt::SmoothTransformation );
    m_pixMapArray[2] = QPixmap(":/img/img/warning.png").scaledToHeight(50, Qt::SmoothTransformation );

    // defaults
    m_bVisible = false;
    m_notifType = NotificationType::NOTIFI_WARNING;
    m_width = 300;
    m_height = 100;
    m_posX =  parentWidget()->geometry().width() - ( m_width + 20);
    m_posY =  parentWidget()->geometry().height() -( m_height + 40);

    m_pGeomeAnimat = new QPropertyAnimation( this, "geometry" );
    connect( m_pGeomeAnimat, SIGNAL(finished()), SLOT(OnFinished()) );

    setGeometry( m_posX, m_posY, m_width, m_height );
    hide();
}

QNotify::~QNotify()
{
    QPixmapCache::clear();
}

void QNotify::SetPosition( QPoint p )
{
    m_posX = p.x();
    m_posY = p.y();
    setGeometry( m_posX, m_posY, m_width, m_height );
    repaint();
}

void QNotify::SetDialogWidth( int width )
{
    m_width = width;
    setGeometry( m_posX, m_posY, m_width, m_height );
    repaint();
}

void QNotify::AdjustInViewport()
{
    m_posX =  parentWidget()->geometry().width() - ( m_width + 20);
    m_posY =  parentWidget()->geometry().height() -( m_height + 30);
    setGeometry( m_posX, m_posY, m_width, m_height );
//    repaint();
}

void QNotify::Notify( QString text, NotificationType type, int duration )
{
    msgText = text;
    m_notifType = type;
    m_keepDuration = duration;
    repaint();

    m_pGeomeAnimat->setDuration(500);
    m_pGeomeAnimat->setStartValue(QRect( m_posX, m_posY + m_height, m_width, m_height ));
    m_pGeomeAnimat->setEndValue  (QRect( m_posX, m_posY, m_width, m_height ));
    m_bVisible = true;
    m_pGeomeAnimat->start();
    show();
}

void QNotify::OnFinished()
{
    if ( m_bVisible ) {
        m_pGeomeAnimat->setStartValue( QRect( m_posX, m_posY, m_width, m_height ) );
        m_pGeomeAnimat->setEndValue( QRect( m_posX, m_posY + m_height, m_width, m_height ) );
        QTimer::singleShot( m_keepDuration, m_pGeomeAnimat, SLOT(start()) );
        m_bVisible = false;
    } else {
        m_bVisible = false;
        hide();
    }
}

void QNotify::paintEvent( QPaintEvent * )
{
    QPainter painter( this );
    painter.setRenderHints( QPainter::HighQualityAntialiasing | QPainter::Qt4CompatiblePainting );
    painter.setBrush(QColor(255,255,255,100)); // 最后一项为透明度
//    painter.setOpacity(0.6);
//    painter.setBrush( Qt::white );

    QFont font;
    font.setFamily("Microsoft YaHei");
    // 大小
    font.setPointSize(12);
    // 斜体
//    font.setItalic(true);
    // 设置下划线
    font.setUnderline(true);
    // 设置上划线
//    font.setOverline(true);
    // 设置字母大小写
//    font.setCapitalization(QFont::SmallCaps);
    // 设置字符间距
//    font.setLetterSpacing(QFont::AbsoluteSpacing, 20);
    // 设置加粗
    font.setBold(true);
    // 使用字体
    painter.setFont(font);
    painter.setPen( Qt::NoPen );
    painter.drawRoundedRect( 0, 0,m_width, m_height, 5.0, 5.0 );
    QString key;
    if ( m_notifType == QNotify::NOTIFI_SUCCESS ) {
        key = "success.png";
    } else if ( m_notifType == QNotify::NOTIFI_ERROR ) {
        key = "error.png";
    } else if ( m_notifType == QNotify::NOTIFI_WARNING ) {
        key = "warning.png";
    }
    #if 0
    QPixmap pixmap;
    int proposedCoord = 10;
    if(QPixmapCache::find(key,&pixmap))
    {
        int pixWidth = pixmap.width();
        int pixheight =pixmap.height();
         proposedCoord = (m_height- pixheight)/2;
         painter.drawPixmap( 10, proposedCoord, pixmap);
         painter.setPen(vetMsgColor.at(m_notifType));
         int remainingWidth = m_width - pixWidth;
         painter.drawText( 10 + pixWidth, proposedCoord, remainingWidth, m_height - 20, Qt::AlignCenter | Qt::TextWordWrap, msgText );
    }
    else
    {
        printf("no fonund QPixmap in cache!!!!\n");
        painter.setPen(vetMsgColor.at(m_notifType) );
        painter.drawText( 0, 10, m_width, m_height - 20, Qt::AlignCenter | Qt::TextWordWrap, msgText );
    }
    #endif

    QPixmap &pixmap = m_pixMapArray[m_notifType];
    int proposedCoord = 10;
    int pixWidth = pixmap.width();
    int pixheight =pixmap.height();
    proposedCoord = (m_height- pixheight)/2;
    painter.drawPixmap( 10, proposedCoord, pixmap);
    painter.setPen(vetMsgColor.at(m_notifType));
    int remainingWidth = m_width - pixWidth;
    painter.drawText( 10 + pixWidth, proposedCoord, remainingWidth, m_height - 20, Qt::AlignCenter | Qt::TextWordWrap, msgText );

}

void QNotify::mousePressEvent( QMouseEvent *mouseEvent )
{
    if ( mouseEvent->buttons() == Qt::LeftButton ) {
        m_pGeomeAnimat->setStartValue( QRect( m_posX, m_posY, m_width, m_height ) );
        m_pGeomeAnimat->setEndValue( QRect( m_posX,m_posY + m_height, m_width, m_height ) );
        m_pGeomeAnimat->start();
        m_bVisible = false;
    }
}
