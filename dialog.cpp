#include "dialog.h"
#include "ui_dialog.h"
#include "QDebug"
#include <libusb.h>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    ui->m_listDevices->setColumnCount(4);
    ui->m_listDevices->setHorizontalHeaderLabels(QStringList()<<"Manufacturer" << "Description" << "Descriptor" << "VID, PID");
    ui->m_listDevices->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->m_listDevices->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->m_listDevices->setEditTriggers(QAbstractItemView::NoEditTriggers);

    if ((m_ftdi = ftdi_new()) == 0) {
        qDebug("ftdi_new failde!");
        return;
    }
}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::on_m_findAll_clicked()
{
    struct ftdi_device_list *devlist, *curdev;
    char manufacturer[128], description[128];
    int retval = EXIT_SUCCESS;

    if ((retval=ftdi_usb_find_all (m_ftdi,&devlist, 0, 0))<0)
    {
        qDebug("ftdi_usb_find_all failed: %s", ftdi_get_error_string(m_ftdi));
        ftdi_free(m_ftdi);
        return;
    }
    qDebug("Number of FTDI devices found: %d\n", retval);

    int i = 0;
    for (curdev = devlist; curdev != NULL; i++)
    {        
        if (ftdi_usb_get_strings(m_ftdi, curdev->dev, manufacturer, sizeof (manufacturer), description, sizeof(description), NULL, 0)<0) {
            qDebug("ftdi_usb_get_strings failed: %s\n", ftdi_get_error_string(m_ftdi));
            return;
        }
        qDebug("Manufacturer: %s, Description: %s\n\n", manufacturer, description);       
        ui->m_listDevices->insertRow(i);
        ui->m_listDevices->setItem(i, 0, new QTableWidgetItem(manufacturer));
        ui->m_listDevices->setItem(i, 1, new QTableWidgetItem(description));


         curdev = curdev->next;

    }
    ftdi_list_free(&devlist);
    return;
}

