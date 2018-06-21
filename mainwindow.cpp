#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QSerialPort serial;
        serial.setPort(info);

        if (serial.open(QIODevice::ReadWrite))
        {
            ui->comboBoxPCPortNum->addItem(info.portName());
            serial.close();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_radioButtonOpenPort_toggled(bool checked)
{
    PCSerialPort = new QSerialPort(this);
    if (checked == true)
    {
        PCSerialPort->open(QIODevice::ReadWrite);
        PCSerialPort->setPortName(ui->comboBoxPCPortNum);
        PCSerialPort->setBaudRate(ui->comboBoxPCBaudRate);
        PCSerialPort->setDataBits(ui->comboBoxPCData);
        PCSerialPort->setParity(ui->comboBoxPCParity);
        PCSerialPort->setFlowControl(ui->comboBoxPCFlowControl);
        PCSerialPort->setStopBits(ui->comboBoxPCStop);
    }
    else if (checked == false)
    {
        PCSerialPort->close();
    }
}
