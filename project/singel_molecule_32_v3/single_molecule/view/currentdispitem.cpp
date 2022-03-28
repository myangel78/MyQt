#include "currentdispitem.h"
#include "ui_currentdispitem.h"
#include <QTime>
#include "Log.h"'
#include <QPainter>
#include <QAction>
#include <QColorDialog>

const QString holeColor[3] = {"background: white","background: green","background: red"};
const QString holeClick  = "border:3px solid black;";
const QString holeSingle = "border:3px solid blue;";


CurrentDispItem::CurrentDispItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurrentDispItem)
{
    ui->setupUi(this);
    m_bZeroSet = false;
    m_holeType = UN_HOLE_STATE;

    ui->colorBtn->setFixedSize(QSize(16, 16));
    ui->colorBtn->setAutoRaise(true);
    QAction *action = new QAction(this);
    action->setToolTip(tr("Change plot lines color"));
    ui->colorBtn->setDefaultAction(action);

    connect(ui->colorBtn, &QToolButton::clicked, this, &CurrentDispItem::GraphChangeColor);
    connect(ui->chcheckBox, &QCheckBox::toggled, this, &CurrentDispItem::GraphChangeVisuable);
}

CurrentDispItem::~CurrentDispItem()
{
    delete ui;
}

void CurrentDispItem::init(int &chanel,const QColor &color)
{
    ui->chcheckBox->setText(QString("CH%1").arg(chanel+1));
    setChannel(chanel);
    setColor(color);
}

void CurrentDispItem::setHoleColor(const enum PORE_STATE &type)
{
    if(!m_bZeroSet)
    {
        ui->holeEmbStateBtn->setStyleSheet(holeColor[type]);
    }
}

void CurrentDispItem::JudgeHoleType(const double &single,const double &mult,const double &value)
{
    if(value < single)
    {
        m_holeType =  UN_HOLE_STATE;
    }
    else if(value > mult)
    {
        m_holeType =  MULTDEM_HOLE_STATE;
    }
    else
    {
        m_holeType = SINGEL_HOLE_STATE;
    }
    setHoleColor(m_holeType);
}

void CurrentDispItem::resetHoleType(void)
{
    m_holeType = UN_HOLE_STATE;
    m_bZeroSet = false;
    setHoleColor(m_holeType);
}

void CurrentDispItem::setCLickHoleColor(void)
{
    if(m_holeType == SINGEL_HOLE_STATE)
        ui->holeEmbStateBtn->setStyleSheet(holeSingle + holeColor[m_holeType]);
    else
        ui->holeEmbStateBtn->setStyleSheet(holeClick + holeColor[m_holeType]);
}

void CurrentDispItem::setInstantValue(const double &Value)
{
    ui->instantDispLab->setText(QString::number(Value,'f',2));

}
void CurrentDispItem::setMeanValue(const double &Value)
{
     ui->meanDispLab->setText(QString::number(Value,'f',2));
}

void CurrentDispItem::setHoleEmbedEnable(bool &Checked)
{
    ui->autoSetZero->setEnabled(Checked);
    ui->holeEmbStateBtn->setEnabled(Checked);
}


void CurrentDispItem::on_holeEmbStateBtn_clicked()
{
    emit setZeroSig(this);
    m_bZeroSet = true;
    setCLickHoleColor();
}

void CurrentDispItem::AutoSetZeroAction(void)
{
    if(ui->autoSetZero->isChecked())
    {
        if(m_holeType != UN_HOLE_STATE  && !m_bZeroSet)
        {
            caliSleep(5);
//            LOGI("m_channel{}",m_channel);
            on_holeEmbStateBtn_clicked();
        }
    }
}


void CurrentDispItem::caliSleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

}




bool CurrentDispItem::isChecked() {
    return ui->chcheckBox->isChecked();
}

QColor CurrentDispItem::color() {
    return Color;
}

void CurrentDispItem::setChecked(bool stats) {
    ui->chcheckBox->setChecked(stats);
}

void CurrentDispItem::setColor(QColor color)
{
    Color = color;
    QPixmap pixmap(16, 16);
    QPainter painter(&pixmap);
    painter.setPen(QColor(50, 50, 50));
    painter.drawRect(QRect(0, 0, 15, 15));
    painter.fillRect(QRect(1, 1, 14, 14), color);

    ui->colorBtn->defaultAction()->setIcon(QIcon(pixmap));
    emit GraphColorChangeSig(m_channel,color);

}

void CurrentDispItem::GraphChangeColor()
{
    QColor color = QColorDialog::getColor(QColor(Color), this);
    if (color.isValid()) {
        setColor(color);
        emit GraphColorChangeSig(m_channel,color);
    }
}

void CurrentDispItem::GraphChangeVisuable(bool checked)
{
    emit  GraphVisibleSig(m_channel,checked);
}
