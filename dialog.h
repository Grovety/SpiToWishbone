#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "ftdi.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

protected:
    ftdi_context* m_ftdi;
    ftdi_device_list* m_devlist;
    ftdi_device_list* m_curdev;

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_m_findAll_clicked();

    void on_m_listDevices_cellDoubleClicked(int row, int column);

    void on_m_testSPI_clicked();

private:
    Ui::Dialog *ui;
};


void SPI_CSEnable();
void SPI_CSDisable();

#endif // DIALOG_H
