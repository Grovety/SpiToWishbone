#include "dialog.h"
#include "ui_dialog.h"
#include "QDebug"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    m_ftdi = ftdi_new();

    switch (ftdi_init(m_ftdi)) {
    case -1:
        qDebug("Error initializing libftdi: couldn't allocate read buffer");
        return;
        break;
    case -2:
        qDebug("Error initializing libftdi: couldn't allocate struct  buffer");
        return;
        break;
    case -3:
        qDebug("Error initializing libftdi: libusb_init() failed");
        return;
        break;
    case 0:
        qDebug("libftdi: libusb sucessfully initialized");
        exit();
        break;
    }

    struct ftdi_device_list *devlist, *curdev;

    static char manufacturer[128], description[128];

    if (ftdi_usb_find_all (m_ftdi,&devlist, 0, 0)>=0)
    {
        int i = 0;
        for (curdev = devlist; curdev != NULL; i++)
        {

            ftdi_usb_get_strings(m_ftdi, curdev->dev, manufacturer, sizeof (manufacturer), description, sizeof(description), NULL, 0);
            int stop = 0;
        }
        ftdi_list_free(&devlist);
    }
}

Dialog::~Dialog()
{
    delete ui;
}

