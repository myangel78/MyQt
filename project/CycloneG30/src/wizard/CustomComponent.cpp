#include "CustomComponent.h"

CustomDoubleSpinbox::CustomDoubleSpinbox(const double &minimum,const double &maximum,const double &curValue,\
                    const double &singleStep,const QString &prefix,const QString &suffix,\
                    QWidget *parent): QDoubleSpinBox(parent)
{
    setRange(minimum,maximum);
    setValue(curValue);
    setSingleStep(singleStep);
    if(!prefix.isEmpty())
    {
        setPrefix(prefix);
    }
    if(!suffix.isEmpty())
    {
        setSuffix(suffix);
    }
}

CustomDoubleSpinbox::~CustomDoubleSpinbox()
{

}

CustomSpinbox::CustomSpinbox(const int &minimum,const int &maximum,const int &curValue,\
                    const int &singleStep,const QString &prefix,const QString &suffix,\
                    QWidget *parent): QSpinBox(parent)
{
    setRange(minimum,maximum);
    setValue(curValue);
    setSingleStep(singleStep);
    if(!prefix.isEmpty())
    {
        setPrefix(prefix);
    }
    if(!suffix.isEmpty())
    {
        setSuffix(suffix);
    }
}

CustomSpinbox::~CustomSpinbox()
{

}
