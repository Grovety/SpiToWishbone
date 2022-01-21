#include "dialog.h"
#include "ui_dialog.h"
#include "QDebug"
#include <libusb.h>
#include <QMessageBox>
#include <QThread>


Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    m_ftdi = 0;
    ui->m_listDevices->setColumnCount(4);
    ui->m_listDevices->setHorizontalHeaderLabels(QStringList()<<"Manufacturer" << "Description" << "Descriptor" << "VID, PID");
    ui->m_listDevices->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->m_listDevices->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->m_listDevices->setEditTriggers(QAbstractItemView::NoEditTriggers);

    if ((m_ftdi = ftdi_new()) == 0) {
        qDebug("ftdi_new failed!");
        return;
    }

    ui->m_memoryDump->setData(m_memoryData);
    on_m_findAll_clicked();
}

Dialog::~Dialog()
{
    delete ui;
    ftdi_list_free(&m_devlist);
}


void Dialog::on_m_findAll_clicked()
{
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
    (void) row;
    (void) column;


}

void Dialog::on_m_testSPI_clicked()
{

    // Now we know that correct address is 0x80000
    for (int shift = 0;shift <1;shift++)
    {
        m_dwNumBytesToSend = 0;

        uint32_t addr = (1<<shift);
        SPI_TlastDisable();
        SPI_CSEnable();

        m_outputBuffer[m_dwNumBytesToSend++] = MPSSE_DO_WRITE | MPSSE_WRITE_NEG | MPSSE_LSB ;
        m_outputBuffer[m_dwNumBytesToSend++] = 0x11; // length low byte, 0x0011 ==> 18dec byte
        m_outputBuffer[m_dwNumBytesToSend++] = 0x00; // length high byte

        m_outputBuffer[m_dwNumBytesToSend++] =0xa2;
        m_outputBuffer[m_dwNumBytesToSend++] = (uint8_t)(addr/0x1000000);
        m_outputBuffer[m_dwNumBytesToSend++] =(uint8_t)(addr/0x10000);
        m_outputBuffer[m_dwNumBytesToSend++] =(uint8_t)(addr/0x100);
        m_outputBuffer[m_dwNumBytesToSend++] =(uint8_t)(addr/0x1);
        m_outputBuffer[m_dwNumBytesToSend++] =0x00;
        m_outputBuffer[m_dwNumBytesToSend++] =0x0c;
        m_outputBuffer[m_dwNumBytesToSend++] =0x11;
        m_outputBuffer[m_dwNumBytesToSend++] =0x22;
        m_outputBuffer[m_dwNumBytesToSend++] =0x33;
        m_outputBuffer[m_dwNumBytesToSend++] =0x44;
        m_outputBuffer[m_dwNumBytesToSend++] =0x55;
        m_outputBuffer[m_dwNumBytesToSend++] =0x66;
        m_outputBuffer[m_dwNumBytesToSend++] =0x77;
        m_outputBuffer[m_dwNumBytesToSend++] =0x88;
        m_outputBuffer[m_dwNumBytesToSend++] =0x99;
        m_outputBuffer[m_dwNumBytesToSend++] =0xaa;
        m_outputBuffer[m_dwNumBytesToSend++] =0xbb;
        SPI_TlastEnable();
        m_outputBuffer[m_dwNumBytesToSend++] = MPSSE_DO_WRITE | MPSSE_WRITE_NEG | MPSSE_LSB ;
        m_outputBuffer[m_dwNumBytesToSend++] = 0x00; // length low byte, 0x0000 ==> 1 byte
        m_outputBuffer[m_dwNumBytesToSend++] = 0x00; // length high byte
        m_outputBuffer[m_dwNumBytesToSend++] =0xcc;

        SPI_CSDisable();
        SPI_TlastDisable();

        SPI_CSEnable();
        m_outputBuffer[m_dwNumBytesToSend++] = MPSSE_DO_WRITE | MPSSE_WRITE_NEG | MPSSE_LSB /*| MPSSE_DO_READ*/;
        m_outputBuffer[m_dwNumBytesToSend++] = 0x05; // length low byte,
        m_outputBuffer[m_dwNumBytesToSend++] = 0x00; // length high byte

/*        m_outputBuffer[m_dwNumBytesToSend++] =0xff;
        m_outputBuffer[m_dwNumBytesToSend++] =0xff;
        m_outputBuffer[m_dwNumBytesToSend++] =0xff;
        m_outputBuffer[m_dwNumBytesToSend++] =0xff;*/


        m_outputBuffer[m_dwNumBytesToSend++] =0xa1;
        m_outputBuffer[m_dwNumBytesToSend++] = (uint8_t)(addr/0x1000000);
        m_outputBuffer[m_dwNumBytesToSend++] =(uint8_t)(addr/0x10000);
        m_outputBuffer[m_dwNumBytesToSend++] =(uint8_t)(addr/0x100);
        m_outputBuffer[m_dwNumBytesToSend++] =(uint8_t)(addr/0x1);
        m_outputBuffer[m_dwNumBytesToSend++] =0x00;

        SPI_TlastEnable();
        m_outputBuffer[m_dwNumBytesToSend++] = MPSSE_DO_WRITE | MPSSE_WRITE_NEG | MPSSE_LSB /*| MPSSE_DO_READ*/;
        m_outputBuffer[m_dwNumBytesToSend++] = 0x00; // length low byte,
        m_outputBuffer[m_dwNumBytesToSend++] = 0x00; // length high byte
        m_outputBuffer[m_dwNumBytesToSend++] = 0x10;
        m_outputBuffer[m_dwNumBytesToSend++] = /*MPSSE_DO_WRITE |*/ MPSSE_WRITE_NEG | MPSSE_LSB | MPSSE_DO_READ;
        m_outputBuffer[m_dwNumBytesToSend++] = 19; // length low byte,
        m_outputBuffer[m_dwNumBytesToSend++] = 0x00; // length high byte

        SPI_TlastDisable();
        SPI_CSDisable();

        ftdi_write_data(m_ftdi, m_outputBuffer, m_dwNumBytesToSend);

        struct ftdi_transfer_control *tc_read;
        tc_read = ftdi_read_data_submit(m_ftdi, m_inputBuffer, 20);
        int transfer = ftdi_transfer_data_done(tc_read);
        int stop = 0;
    }

}

//this routine is used to enable SPI device
void Dialog::SPI_CSEnable() {
    //one 0x80 command can keep 0.2us, do 5 times to stay in this situation for 1us
    for (int loop = 0; loop < 5; loop++) {
        m_outputBuffer[m_dwNumBytesToSend++] = '\x80';//GPIO command for ADBUS
        m_outputBuffer[m_dwNumBytesToSend++] = '\x00';//set CS high, MOSI and SCL low
        m_outputBuffer[m_dwNumBytesToSend++] = '\x1b';//bit3:CS, bit2:MISO, bit1:MOSI, bit0 : SCK
    }
}


//this routine is used to disable SPI device
void Dialog::SPI_CSDisable() {
    //one 0x80 command can keep 0.2us, do 5 times to stay in this situation for 1us
    for (int loop = 0; loop < 5; loop++) {
        m_outputBuffer[m_dwNumBytesToSend++] = '\x80';//GPIO command for ADBUS
        m_outputBuffer[m_dwNumBytesToSend++] = '\x08';//set CS, MOSI and SCL low
        m_outputBuffer[m_dwNumBytesToSend++] = '\x1b';//bit3:CS, bit2:MISO, bit1:MOSI, bit0 : SCK
    }
}
void Dialog::SPI_TlastEnable() {
    //one 0x80 command can keep 0.2us, do 5 times to stay in this situation for 1us
        m_outputBuffer[m_dwNumBytesToSend++] = '\x80';//GPIO command for ADBUS
        m_outputBuffer[m_dwNumBytesToSend++] = '\x10';//set CS high, MOSI and SCL low
        m_outputBuffer[m_dwNumBytesToSend++] = '\x1b';//bit3:CS, bit2:MISO, bit1:MOSI, bit0 : SCK
}


//this routine is used to disable SPI device
void Dialog::SPI_TlastDisable() {
    //one 0x80 command can keep 0.2us, do 5 times to stay in this situation for 1us
        m_outputBuffer[m_dwNumBytesToSend++] = '\x80';//GPIO command for ADBUS
        m_outputBuffer[m_dwNumBytesToSend++] = '\x00';//set CS, MOSI and SCL low
        m_outputBuffer[m_dwNumBytesToSend++] = '\x1b';//bit3:CS, bit2:MISO, bit1:MOSI, bit0 : SCK
}

void Dialog::SPI_Send(uint8_t data)
{

}

bool Dialog::WishBoneReadMemory(uint32_t addr, uint32_t len, uint8_t* pData)
{
    if ((m_ftdi== 0) || (m_ftdi->usb_dev == 0))
    {
        return false;
    }
    uint32_t actualAddr = addr/4;
    uint32_t actualLen = len + 8;

    m_dwNumBytesToSend = 0;


    SPI_TlastDisable();
    SPI_CSEnable();
    m_outputBuffer[m_dwNumBytesToSend++] = MPSSE_DO_WRITE | MPSSE_WRITE_NEG | MPSSE_LSB /*| MPSSE_DO_READ*/;
    m_outputBuffer[m_dwNumBytesToSend++] = 0x05; // length low byte,
    m_outputBuffer[m_dwNumBytesToSend++] = 0x00; // length high byte

    m_outputBuffer[m_dwNumBytesToSend++] =0xa1;
    m_outputBuffer[m_dwNumBytesToSend++] = (uint8_t)(actualAddr/0x1000000);
    m_outputBuffer[m_dwNumBytesToSend++] =(uint8_t)(actualAddr/0x10000);
    m_outputBuffer[m_dwNumBytesToSend++] =(uint8_t)(actualAddr/0x100);
    m_outputBuffer[m_dwNumBytesToSend++] =(uint8_t)(actualAddr/0x1);

    m_outputBuffer[m_dwNumBytesToSend++] =(uint8_t)(len/0x100);

    SPI_TlastEnable();
    m_outputBuffer[m_dwNumBytesToSend++] = MPSSE_DO_WRITE | MPSSE_WRITE_NEG | MPSSE_LSB /*| MPSSE_DO_READ*/;
    m_outputBuffer[m_dwNumBytesToSend++] = 0x00; // length low byte,
    m_outputBuffer[m_dwNumBytesToSend++] = 0x00; // length high byte
    m_outputBuffer[m_dwNumBytesToSend++] = (uint8_t)(len/0x1);
    m_outputBuffer[m_dwNumBytesToSend++] = /*MPSSE_DO_WRITE |*/ MPSSE_WRITE_NEG | MPSSE_LSB | MPSSE_DO_READ;
    m_outputBuffer[m_dwNumBytesToSend++] = (uint8_t)(actualLen/0x1); // length low byte,
    m_outputBuffer[m_dwNumBytesToSend++] = (uint8_t)(actualLen/0x100); // length high byte
    SPI_TlastDisable();
    SPI_CSDisable();

    ftdi_write_data(m_ftdi, m_outputBuffer, m_dwNumBytesToSend);

    struct ftdi_transfer_control *tc_read;
    tc_read = ftdi_read_data_submit(m_ftdi, m_inputBuffer, actualLen+1);
    int transfer = ftdi_transfer_data_done(tc_read);
    if (m_inputBuffer[0] != (uint8_t)len)
    {
        return false;
    }
    memcpy (pData,m_inputBuffer+1,len);
    return true;

}

bool Dialog::WishBoneWriteMemory(uint32_t addr, uint32_t len, uint8_t *pData)
{
    if ((m_ftdi== 0) || (m_ftdi->usb_dev == 0))
    {
        return false;
    }
    uint32_t actualAddr = addr/4;
    uint32_t lenOfFirstPart = 0x07 + len - 2;

    m_dwNumBytesToSend = 0;


    SPI_TlastDisable();
    SPI_CSEnable();
    m_outputBuffer[m_dwNumBytesToSend++] = MPSSE_DO_WRITE | MPSSE_WRITE_NEG | MPSSE_LSB /*| MPSSE_DO_READ*/;
    m_outputBuffer[m_dwNumBytesToSend++] = (uint8_t)(lenOfFirstPart/0x1); // length low byte,
    m_outputBuffer[m_dwNumBytesToSend++] = (uint8_t)(lenOfFirstPart/0x100); // length high byte

    m_outputBuffer[m_dwNumBytesToSend++] =0xa2;
    m_outputBuffer[m_dwNumBytesToSend++] = (uint8_t)(actualAddr/0x1000000);
    m_outputBuffer[m_dwNumBytesToSend++] =(uint8_t)(actualAddr/0x10000);
    m_outputBuffer[m_dwNumBytesToSend++] =(uint8_t)(actualAddr/0x100);
    m_outputBuffer[m_dwNumBytesToSend++] =(uint8_t)(actualAddr/0x1);

    m_outputBuffer[m_dwNumBytesToSend++] =(uint8_t)(len/0x100);
    m_outputBuffer[m_dwNumBytesToSend++] = (uint8_t)(len/0x1);

    for (uint32_t i = 0; i<len-1;i++)
    {
        m_outputBuffer[m_dwNumBytesToSend++] = pData [i];
    }

    SPI_TlastEnable();
    m_outputBuffer[m_dwNumBytesToSend++] = MPSSE_DO_WRITE | MPSSE_WRITE_NEG | MPSSE_LSB /*| MPSSE_DO_READ*/;
    m_outputBuffer[m_dwNumBytesToSend++] = 0x03; // length low byte,
    m_outputBuffer[m_dwNumBytesToSend++] = 0x00; // length high byte
    m_outputBuffer[m_dwNumBytesToSend++] = pData [len-1];
    m_outputBuffer[m_dwNumBytesToSend++] = 0xff;
    m_outputBuffer[m_dwNumBytesToSend++] = 0xff;
    m_outputBuffer[m_dwNumBytesToSend++] = 0xff;
    SPI_TlastDisable();
    SPI_CSDisable();

//    ftdi_write_data(m_ftdi, m_outputBuffer, m_dwNumBytesToSend);

    struct ftdi_transfer_control *tc_write;
    tc_write = ftdi_write_data_submit(m_ftdi, m_outputBuffer, m_dwNumBytesToSend);
    int transfer = ftdi_transfer_data_done(tc_write);

    return true;

}
namespace Pin {
   // enumerate the AD bus for conveniance.
   enum bus_t {
      SK = 0x01, // ADBUS0, SPI data clock
      DO = 0x02, // ADBUS1, SPI data out
      DI = 0x04, // ADBUS2, SPI data in
      CS = 0x08, // ADBUS3, SPI chip select
      L0 = 0x10, // ADBUS4, general-ourpose i/o, GPIOL0
      L1 = 0x20, // ADBUS5, general-ourpose i/o, GPIOL1
      L2 = 0x40, // ADBUS6, general-ourpose i/o, GPIOL2
      l3 = 0x80  // ADBUS7, general-ourpose i/o, GPIOL3
   };

}void Dialog::on_pushButton_clicked()
{

    // Set these pins high
    const unsigned char pinInitialState = Pin::CS|Pin::L0|Pin::L1;
    // Use these pins as outputs
    const unsigned char pinDirection    = Pin::SK|Pin::DO|Pin::CS|Pin::L0|Pin::L1;


    int baudrate = 2000000;

    auto items = ui->m_listDevices->selectedItems();
    if (items.count()==0)
    {
        QMessageBox::warning(this,"Problem","Please, select any device");
        return;
    }
    int row = items.at(0)->row();
    int status;
    // Set m_curdev to selected device
    m_curdev = m_devlist;
    for (int i = 0; i < row; i++ ) {
        m_curdev = m_curdev->next;
    }

    status = ftdi_usb_open_dev(m_ftdi, m_curdev->dev);
    if (status <0) {
        qDebug("Unable to open device %s", ftdi_get_error_string(m_ftdi));
        ui->pushButton->setIcon(QIcon(":/new/icons/Pictures/LedRed.png"));
        return;
    }

    // This code is copied from example
    ftdi_usb_reset(m_ftdi);
    ftdi_set_interface(m_ftdi, INTERFACE_ANY);
    ftdi_set_bitmode(m_ftdi, 0, 0); // reset
    ftdi_set_bitmode(m_ftdi, 0, BITMODE_MPSSE); // enable mpsse on all bits
    ftdi_usb_purge_buffers(m_ftdi);
    QThread::usleep(50000); // sleep 50 ms for setup to complete
    m_dwNumBytesToSend = 0;
    m_outputBuffer[m_dwNumBytesToSend++] = TCK_DIVISOR;     // opcode: set clk divisor
    m_outputBuffer[m_dwNumBytesToSend++] = 0x05;            // argument: low bit. 60 MHz / (5+1) = 1 MHz
    m_outputBuffer[m_dwNumBytesToSend++] = 0x00;            // argument: high bit.
    m_outputBuffer[m_dwNumBytesToSend++] = DIS_ADAPTIVE;    // opcode: disable adaptive clocking
    m_outputBuffer[m_dwNumBytesToSend++] = DIS_3_PHASE;     // opcode: disable 3-phase clocking
    m_outputBuffer[m_dwNumBytesToSend++] = SET_BITS_LOW;    // opcode: set low bits (ADBUS[0-7])
    m_outputBuffer[m_dwNumBytesToSend++] = pinInitialState; // argument: inital pin states
    m_outputBuffer[m_dwNumBytesToSend++] = pinDirection;    // argument: pin direction
    // Write the setup to the chip.
    if ( ftdi_write_data(m_ftdi, m_outputBuffer, m_dwNumBytesToSend) != m_dwNumBytesToSend ) {
       QMessageBox::warning(this,"Problem","Write failed");
       return;
    }

    // zero the buffer for good measure
    memset(m_outputBuffer, 0, sizeof(m_outputBuffer));
    m_dwNumBytesToSend = 0;

    // Now we will write and read 1 byte.
    // The DO and DI pins should be physically connected on the breadboard.
    // Next three commands sets the GPIOL0 pin low. Pulling CS low.
    m_outputBuffer[m_dwNumBytesToSend++] = SET_BITS_LOW;
    m_outputBuffer[m_dwNumBytesToSend++] = pinInitialState & ~Pin::CS;
    m_outputBuffer[m_dwNumBytesToSend++] = pinDirection;
    // commands to write and read one byte in SPI0 (polarity = phase = 0) mode
    m_outputBuffer[m_dwNumBytesToSend++] = MPSSE_DO_WRITE | MPSSE_WRITE_NEG | MPSSE_DO_READ;
    m_outputBuffer[m_dwNumBytesToSend++] = 0x00; // length low byte, 0x0000 ==> 1 byte
    m_outputBuffer[m_dwNumBytesToSend++] = 0x00; // length high byte
    m_outputBuffer[m_dwNumBytesToSend++] = 0x12; // byte to send
    // Next three commands sets the GPIOL0 pin high. Pulling CS high.
    m_outputBuffer[m_dwNumBytesToSend++] = SET_BITS_LOW;
    m_outputBuffer[m_dwNumBytesToSend++] = pinInitialState | Pin::CS;
    m_outputBuffer[m_dwNumBytesToSend++] = pinDirection;
/*    std::cout << "Writing: ";
    for ( int i = 0; i < m_dwNumBytesToSend; ++i ) {
      std::cout << std::hex << (unsigned int)m_outputBuffer[i] << ' ';
    }
    std::cout << '\n';
    // need to purge tx when reading for some etherial reason
    ftdi_usb_purge_tx_buffer(&ftdi);
    if ( ftdi_write_data(&ftdi, buf, m_dwNumBytesToSend) != m_dwNumBytesToSend ) {
       std::cout << "Write failed\n";
    }
    // zero the buffer for good measure
    memset(m_outputBuffer, 0, sizeof(m_outputBuffer));
    m_dwNumBytesToSend = 0;

    // now get the data we read just read from the chip
    unsigned char readm_outputBuffer[256] = {0};
    if ( ftdi_read_data(&ftdi, readBuf, 1) != 1 ) std::cout << "Read failed\n";
    else std::cout << "Answer: " << std::hex << (unsigned int)readm_outputBuffer[0] << '\n';*/

    // to here

/*    ftdi_set_baudrate(m_ftdi, baudrate);
    if (ftdi_set_bitmode(m_ftdi, 0xFF, BITMODE_MPSSE) < 0) {
        qDebug("Can't set mode: %s\n",ftdi_get_error_string(m_ftdi));
        ui->pushButton->setIcon(QIcon(":/new/icons/Pictures/LedRed.png"));
        return;
    }

    // Test if MPSSE mode is active
    dwNumBytesToSend = 0;
    OutputBuffer[m_dwNumBytesToSend++] = '\xAA';
    ftdi_write_data(m_ftdi, OutputBuffer, dwNumBytesToSend);
    dwNumBytesToSend = 0;*/
    ui->pushButton->setIcon(QIcon(":/new/icons/Pictures/LedGreen.png"));

}

void Dialog::on_pushButton_2_clicked()
{
    // Now we know that correct address is 0x80000
    m_dwNumBytesToSend = 0;

    uint32_t addr = 0xf0003000/4;
    SPI_TlastDisable();
    SPI_CSEnable();

    m_outputBuffer[m_dwNumBytesToSend++] = MPSSE_DO_WRITE | MPSSE_WRITE_NEG | MPSSE_LSB /*| MPSSE_DO_READ*/;
    m_outputBuffer[m_dwNumBytesToSend++] = 0x09; // length low byte, 0x0011 ==> 18dec byte
    m_outputBuffer[m_dwNumBytesToSend++] = 0x00; // length high byte

    m_outputBuffer[m_dwNumBytesToSend++] =0xa2;
    m_outputBuffer[m_dwNumBytesToSend++] = (uint8_t)(addr/0x1000000);
    m_outputBuffer[m_dwNumBytesToSend++] =(uint8_t)(addr/0x10000);
    m_outputBuffer[m_dwNumBytesToSend++] =(uint8_t)(addr/0x100);
    m_outputBuffer[m_dwNumBytesToSend++] =(uint8_t)(addr/0x1);
    m_outputBuffer[m_dwNumBytesToSend++] =0x00;
    m_outputBuffer[m_dwNumBytesToSend++] =0x04;
    m_outputBuffer[m_dwNumBytesToSend++] =0x55;
    m_outputBuffer[m_dwNumBytesToSend++] =0x55;
    m_outputBuffer[m_dwNumBytesToSend++] =0x55;
    SPI_TlastEnable();
    m_outputBuffer[m_dwNumBytesToSend++] = MPSSE_DO_WRITE | MPSSE_WRITE_NEG | MPSSE_LSB /*| MPSSE_DO_READ*/;
    m_outputBuffer[m_dwNumBytesToSend++] = 0x00; // length low byte, 0x0000 ==> 1 byte
    m_outputBuffer[m_dwNumBytesToSend++] = 0x00; // length high byte
    m_outputBuffer[m_dwNumBytesToSend++] =0x55;

    SPI_CSDisable();
    SPI_TlastDisable();

    ftdi_write_data(m_ftdi, m_outputBuffer, m_dwNumBytesToSend);

    int stop = 0;
}

void Dialog::on_m_btnMemoryRead_clicked()
{
    int addr = ui->m_editMemoryAddr->text().toULong(nullptr,0);
    int len = ui->m_editMemoryCnt->text().toULong(nullptr,0);
    m_memoryData.resize(len);
    if (WishBoneReadMemory(addr,len,(uint8_t*) m_memoryData.constData()))
    {
        ui->m_btnMemoryRead->setIcon(QIcon(":/new/icons/Pictures/LedGreen.png"));
    } else
    {
        ui->m_btnMemoryRead->setIcon(QIcon(":/new/icons/Pictures/LedRed.png"));
    }
    ui->m_memoryDump->setData(m_memoryData);
}

void Dialog::on_m_btnFlushSPI_clicked()
{
    if (m_ftdi== 0)
    {
        return;
    }
    m_dwNumBytesToSend = 0;
    SPI_CSEnable();
    SPI_TlastEnable();
    m_outputBuffer[m_dwNumBytesToSend++] = MPSSE_DO_WRITE | MPSSE_WRITE_NEG | MPSSE_LSB /*| MPSSE_DO_READ*/;
    m_outputBuffer[m_dwNumBytesToSend++] = 0x05; // length low byte,
    m_outputBuffer[m_dwNumBytesToSend++] = 0x00; // length high byte

    m_outputBuffer[m_dwNumBytesToSend++] = 0xff;
    m_outputBuffer[m_dwNumBytesToSend++] = 0xff;
    m_outputBuffer[m_dwNumBytesToSend++] = 0xff;
    m_outputBuffer[m_dwNumBytesToSend++] = 0xff;
    m_outputBuffer[m_dwNumBytesToSend++] = 0xff;
    m_outputBuffer[m_dwNumBytesToSend++] = 0xff;
    SPI_CSDisable();
    SPI_TlastDisable();
    ftdi_write_data(m_ftdi, m_outputBuffer, m_dwNumBytesToSend);

}

void Dialog::on_m_btmMemoryWrite_clicked()
{
    int addr = ui->m_editMemoryAddr->text().toULong(nullptr,0);
    int len = ui->m_editMemoryCnt->text().toULong(nullptr,0);
    if (WishBoneWriteMemory(addr,len,(uint8_t*) ui->m_memoryDump->data().constData()))
    {
        ui->m_btmMemoryWrite->setIcon(QIcon(":/new/icons/Pictures/LedGreen.png"));
    } else
    {
        ui->m_btmMemoryWrite->setIcon(QIcon(":/new/icons/Pictures/LedRed.png"));
    }

}
