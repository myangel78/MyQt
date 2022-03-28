#ifndef PLOTTRANSTOTEXTDIALOG_H
#define PLOTTRANSTOTEXTDIALOG_H

#include <QDialog>
#include <QtCharts>

namespace Ui {
class PlotTransToTextDialog;
}

class PlotTransToTextDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlotTransToTextDialog(QWidget *parent = nullptr);
    ~PlotTransToTextDialog();

private slots:
    void on_openBinFileBtn_clicked();
    bool openTextByIODevice(const QString &aFileName);

    void on_openTextBtn_clicked();

private:
    Ui::PlotTransToTextDialog *ui;
};

#endif // PLOTTRANSTOTEXTDIALOG_H
