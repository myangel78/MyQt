#include "MainWindow.h"
#include "BarChartWidget.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    InitCtrl();

    resize(600,480);

}

MainWindow::~MainWindow()
{
}


void MainWindow::InitCtrl(void)
{
    m_pBarChartWidget = new BarChartWidget(this);
    setCentralWidget(m_pBarChartWidget);
}
