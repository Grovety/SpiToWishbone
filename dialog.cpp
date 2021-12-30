#include "dialog.h"
#include "ui_dialog.h"
#include "QDebug"
#include <libusb.h>


#define MemSize 16 //define data quantity you want to send out
const BYTE SPIDATALENGTH = 11;//3 digit command + 8 digit address
const BYTE READ = '\xC0';//110xxxxx
const BYTE WRITE = '\xA0';//101xxxxx
const BYTE WREN = '\x98';//10011xxx
const BYTE ERAL = '\x90';//10010xxx

//declare for BAD command
const BYTE AA_ECHO_CMD_1 = '\xAA';
const BYTE AB_ECHO_CMD_2 = '\xAB';
const BYTE BAD_COMMAND_RESPONSE = '\xFA';
//declare for MPSSE command
const BYTE MSB_RISING_EDGE_CLOCK_BYTE_OUT = '\x10';
const BYTE MSB_FALLING_EDGE_CLOCK_BYTE_OUT = '\x11';
const BYTE MSB_RISING_EDGE_CLOCK_BIT_OUT = '\x12';
const BYTE MSB_FALLING_EDGE_CLOCK_BIT_OUT = '\x13';
const BYTE MSB_RISING_EDGE_CLOCK_BYTE_IN = '\x20';
const BYTE MSB_RISING_EDGE_CLOCK_BIT_IN = '\x22';
const BYTE MSB_FALLING_EDGE_CLOCK_BYTE_IN = '\x24';
const BYTE MSB_FALLING_EDGE_CLOCK_BIT_IN = '\x26';

BYTE OutputBuffer[512]; //Buffer to hold MPSSE commands and data to be sent to FT2232H
BYTE InputBuffer[512]; //Buffer to hold Data bytes to be read from FT2232H
DWORD dwClockDivisor = 29; //Value of clock divisor, SCL Frequency = 60 / ((1 + 29) * 2) (MHz) = 1Mhz
DWORD dwNumBytesToSend = 0; //Index of output buffer
DWORD dwNumBytesSent = 0, dwNumBytesRead = 0, dwNumInputBuffer = 0;

BYTE ByteDataRead;
WORD MemAddress = 0x00;
WORD DataOutBuffer[MemSize];
WORD DataInBuffer[MemSize];


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
        qDebug("ftdi_new failed!");
        return;
    }
}

Dialog::~Dialog()
{
    delete ui;
    ftdi_list_free(&m_devlist);
}


void Dialog::on_m_findAll_clicked()
{
    ui->m_listDevices->clear();
    ui->m_listDevices->setRowCount(0);
    char manufacturer[128], description[128];
    int retval = EXIT_SUCCESS;

    if ((retval=ftdi_usb_find_all (m_ftdi,&m_devlist, 0, 0))<0)
    {
        qDebug("ftdi_usb_find_all failed: %s", ftdi_get_error_string(m_ftdi));
        ftdi_free(m_ftdi);
        return;
    }
    qDebug("Number of FTDI devices found: %d\n", retval);

    int i = 0;
    for (m_curdev = m_devlist; m_curdev != NULL; i++)
    {        
        if (ftdi_usb_get_strings(m_ftdi, m_curdev->dev, manufacturer, sizeof (manufacturer), description, sizeof(description), NULL, 0)<0) {
            qDebug("ftdi_usb_get_strings failed: %s\n", ftdi_get_error_string(m_ftdi));
            return;
        }
        qDebug("Manufacturer: %s, Description: %s\n\n", manufacturer, description);       
        ui->m_listDevices->insertRow(i);
        ui->m_listDevices->setItem(i, 0, new QTableWidgetItem(manufacturer));
        ui->m_listDevices->setItem(i, 1, new QTableWidgetItem(description));
        m_curdev = m_curdev->next;
    }

    return;
}


void Dialog::on_m_listDevices_cellDoubleClicked(int row, int column)
{
    int baudrate = 9600; // ToDo Try rate bigger than 9600

    int status;
    // Set m_curdev to selected device
    m_curdev = m_devlist;
    for (int i = 0; i < row; i++ ) {
        m_curdev = m_curdev->next;
    }

    status = ftdi_usb_open_dev(m_ftdi, m_curdev->dev);
    if (status <0) {
        qDebug("Unable to open device %s", ftdi_get_error_string(m_ftdi));
    }

    ftdi_set_baudrate(m_ftdi, baudrate);
    if (ftdi_set_bitmode(m_ftdi, 0xFF, BITMODE_MPSSE) < 0) {
        qDebug("Can't set mode: %s\n",ftdi_get_error_string(m_ftdi));
    }

    // Test if MPSSE mode is active
    dwNumBytesToSend = 0;
    OutputBuffer[dwNumBytesToSend++] = '\xAA';
    ftdi_write_data(m_ftdi, OutputBuffer, dwNumBytesToSend);
    dwNumBytesToSend = 0;


}


void Dialog::on_m_testSPI_clicked()
{
    SPI_CSEnable();
    SPI_CSDisable();

}

//this routine is used to enable SPI device
void SPI_CSEnable() {
    //one 0x80 command can keep 0.2us, do 5 times to stay in this situation for 1us
    for (int loop = 0; loop < 5; loop++) {
        OutputBuffer[dwNumBytesToSend++] = '\x80';//GPIO command for ADBUS
        OutputBuffer[dwNumBytesToSend++] = '\x08';//set CS high, MOSI and SCL low
        OutputBuffer[dwNumBytesToSend++] = '\x0b';//bit3:CS, bit2:MISO, bit1:MOSI, bit0 : SCK
    }
}


//this routine is used to disable SPI device
void SPI_CSDisable() {
    //one 0x80 command can keep 0.2us, do 5 times to stay in this situation for 1us
    for (int loop = 0; loop < 5; loop++) {
        OutputBuffer[dwNumBytesToSend++] = '\x80';//GPIO command for ADBUS
        OutputBuffer[dwNumBytesToSend++] = '\x00';//set CS, MOSI and SCL low
        OutputBuffer[dwNumBytesToSend++] = '\x0b';//bit3:CS, bit2:MISO, bit1:MOSI, bit0 : SCK
    }
}
