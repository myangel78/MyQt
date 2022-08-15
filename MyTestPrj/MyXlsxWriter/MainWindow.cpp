#include "MainWindow.h"
#include "xlsxwriter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    Init();
}

MainWindow::~MainWindow()
{
}


void MainWindow::Init(void)
{
    /* Create a new workbook and add a worksheet. */
    lxw_workbook  *workbook  = workbook_new("demo.xlsx");
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);
    lxw_worksheet *worksheetTwo = workbook_add_worksheet(workbook, "second");


    /* Add a format. */
    lxw_format *format = workbook_add_format(workbook);

    /* Set the bold property for the format */
    format_set_bold(format);
    format_set_font_color(format, 0x3030E0);

    /* Change the column width for clarity. */
    worksheet_set_column(worksheet, 0, 0, 20, NULL);

    /* Write some simple text. */
    worksheet_write_string(worksheet, 0, 0, "Hello", NULL);

    /* Text with formatting. */
    worksheet_write_string(worksheet, 1, 0, "World", format);

    /* Write some numbers. */
    worksheet_write_number(worksheet, 2, 0, 123,     NULL);
    worksheet_write_number(worksheet, 3, 0, 123.456, NULL);

    worksheet_write_string(worksheetTwo, 0, 0, "Hello", NULL);
    worksheet_write_number(worksheetTwo, 2, 0, 786.034, NULL);
    worksheet_write_number(worksheetTwo, 4, 0, 796.56, NULL);

    /* Insert an image. */
//    worksheet_insert_image(worksheet, 1, 2, "E:/project/personnal/MyXlsxWriter/res/img/ip.png");
    worksheet_insert_image(worksheet, 1, 2, QString("ip.png").toStdString().c_str());

    workbook_close(workbook);

}
