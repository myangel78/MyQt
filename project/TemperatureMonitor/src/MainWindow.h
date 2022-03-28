#pragma once

#include <QMainWindow>
class TemperatureMonitor;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    QMenu* m_pMenuHelp = nullptr;
    QAction* m_pactAbout = nullptr;

    TemperatureMonitor *m_pTemperatureMonitor = nullptr;
private:
    void InitCtrl(void);
    void LoadStyleSheet(const QString &style, QWidget* pObject = nullptr);

private slots:
    void OnShowAbout(void);
};
