#pragma once

#include <QWidget>
#include <QDoubleSpinBox>

class CustomDoubleSpinbox:public QDoubleSpinBox
{
    Q_OBJECT
public:
    CustomDoubleSpinbox() = delete ;
    CustomDoubleSpinbox(const double &minimum,const double &maximum,const double &curValue,\
                        const double &singleStep,const QString &prefix,const QString &suffix,\
                        QWidget *parent);
    ~CustomDoubleSpinbox();
};


class CustomSpinbox:public QSpinBox
{
    Q_OBJECT
public:
    CustomSpinbox() = delete ;
    CustomSpinbox(const int &minimum,const int &maximum,const int &curValue,\
                  const int &singleStep,const QString &prefix,const QString &suffix,\
                  QWidget *parent);
    ~CustomSpinbox();
};

