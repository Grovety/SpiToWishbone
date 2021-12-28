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

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_m_findAll_clicked();

private:
    Ui::Dialog *ui;
};
#endif // DIALOG_H
