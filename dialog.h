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

    BYTE m_outputBuffer[512]; //Buffer to hold MPSSE commands and data to be sent to FT2232H
    BYTE m_inputBuffer[512]; //Buffer to hold Data bytes to be read from FT2232H
    DWORD m_dwNumBytesSent = 0, m_dwNumBytesRead = 0, m_dwNumInputBuffer = 0;
    DWORD m_dwNumBytesToSend;

    QByteArray m_memoryData;

    void SPI_CSEnable();
    void SPI_CSDisable();
    void SPI_TlastEnable();
    void SPI_TlastDisable();
    void SPI_Send(uint8_t data);

public:
    virtual bool WishBoneReadMemory (uint32_t addr, uint32_t len, uint8_t* pData);
    virtual bool WishBoneWriteMemory (uint32_t addr, uint32_t len, uint8_t* pData);
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_m_findAll_clicked();

    void on_m_listDevices_cellDoubleClicked(int row, int column);

    void on_m_testSPI_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_m_btnMemoryRead_clicked();

    void on_m_btnFlushSPI_clicked();

    void on_m_btmMemoryWrite_clicked();

private:
    Ui::Dialog *ui;
};


#endif // DIALOG_H
