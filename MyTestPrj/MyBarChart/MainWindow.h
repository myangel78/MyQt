#pragma once

#include <QMainWindow>
class BarChartWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void InitCtrl(void);
    BarChartWidget *m_pBarChartWidget = nullptr;
};
