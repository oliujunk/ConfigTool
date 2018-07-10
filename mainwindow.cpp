#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
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

void MainWindow::readData()
{
    if(ReceWindowHexState == Qt::Unchecked)
    {
        QByteArray buf;
        buf = PCSerialPort.readAll();
        if(!buf.isEmpty())
        {
            ui->textEditRecv->setTextColor(QColor("green"));
            ui->textEditRecv->insertPlainText(QString(buf));
        }
    }
    else if(ReceWindowHexState == Qt::Checked)
    {
        QByteArray temp = PCSerialPort.readAll();
        QDataStream out(&temp, QIODevice::ReadWrite);    //将字节数组读入
        while(!out.atEnd())
        {
            qint8 outChar = 0;
            out>>outChar;   //每字节填充一次，直到结束
            //十六进制的转换
            QString str = QString("%1").arg(outChar&0xFF,2,16,QLatin1Char('0'));
            ui->textEditRecv->insertPlainText(str.toUpper());//大写
            ui->textEditRecv->insertPlainText(" ");//每发送两个字符后添加一个空格
            ui->textEditRecv->moveCursor(QTextCursor::End);
        }
        temp.clear();
    }
}

void MainWindow::sendOneCmd(QString str)
{
    ui->textEditRecv->setTextColor(QColor("blue"));
    ui->textEditRecv->insertPlainText(str);
    ui->textEditRecv->insertPlainText("\n");
    PCSerialPort.write(QString(str).toLatin1());
}

void MainWindow::on_pushButtonOpenPort_clicked()
{
    if (ui->pushButtonOpenPort->text()==tr("打开串口"))
    {
        PCSerialPort.setPortName(ui->comboBoxPCPortNum->currentText());
        PCSerialPort.open(QIODevice::ReadWrite);
        PCSerialPort.setBaudRate(ui->comboBoxPCBaudRate->currentText().toInt());
        switch (ui->comboBoxPCData->currentIndex())
        {
            case 0:
                PCSerialPort.setDataBits(QSerialPort::Data5);
                break;

            case 1:
                PCSerialPort.setDataBits(QSerialPort::Data6);
                break;
            case 2:
                PCSerialPort.setDataBits(QSerialPort::Data7);
                break;
            case 3:
                PCSerialPort.setDataBits(QSerialPort::Data8);
                break;
            default:
                break;
        }
        switch (ui->comboBoxPCParity->currentIndex())
        {
            case 0:
                PCSerialPort.setParity(QSerialPort::NoParity);
                break;
            case 1:
                PCSerialPort.setParity(QSerialPort::OddParity);
                break;
            case 2:
                PCSerialPort.setParity(QSerialPort::EvenParity);
                break;
            default:
                break;
        }
        switch (ui->comboBoxPCFlowControl->currentIndex())
        {
            case 0:
                PCSerialPort.setFlowControl(QSerialPort::NoFlowControl);
                break;
            case 1:
                PCSerialPort.setFlowControl(QSerialPort::HardwareControl);
                break;
            case 2:
                PCSerialPort.setFlowControl(QSerialPort::SoftwareControl);
                break;
            default:
                break;
        }
        switch (ui->comboBoxPCStop->currentIndex())
        {
            case 0:
                PCSerialPort.setStopBits(QSerialPort::OneStop);
                break;
            case 1:
                PCSerialPort.setStopBits(QSerialPort::OneAndHalfStop);
                break;
            case 2:
                PCSerialPort.setStopBits(QSerialPort::TwoStop);
                break;
            default:
                break;
        }

        ui->comboBoxPCPortNum->setEnabled(false);
        ui->comboBoxPCBaudRate->setEnabled(false);
        ui->comboBoxPCData->setEnabled(false);
        ui->comboBoxPCParity->setEnabled(false);
        ui->comboBoxPCStop->setEnabled(false);
        ui->comboBoxPCFlowControl->setEnabled(false);

        ui->pushButtonEnterATMode->setEnabled(true);
        ui->pushButtonExitATMode->setEnabled(true);
        ui->pushButtonGetPara->setEnabled(true);
        ui->pushButtonSetPara->setEnabled(true);
        ui->pushButtonSend->setEnabled(true);

        ui->pushButtonOpenPort->setText(tr("关闭串口"));

        connect(&PCSerialPort, &QSerialPort::readyRead, this, &MainWindow::readData);

    }
    else
    {
        PCSerialPort.clear();
        PCSerialPort.close();

        ui->comboBoxPCPortNum->setEnabled(true);
        ui->comboBoxPCBaudRate->setEnabled(true);
        ui->comboBoxPCData->setEnabled(true);
        ui->comboBoxPCParity->setEnabled(true);
        ui->comboBoxPCStop->setEnabled(true);
        ui->comboBoxPCFlowControl->setEnabled(true);

        ui->pushButtonEnterATMode->setEnabled(false);
        ui->pushButtonExitATMode->setEnabled(false);
        ui->pushButtonGetPara->setEnabled(false);
        ui->pushButtonSetPara->setEnabled(false);
        ui->pushButtonSend->setEnabled(false);

        ui->pushButtonOpenPort->setText(tr("打开串口"));
    }
}

void MainWindow::on_checkBoxRecvHex_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked)
        ReceWindowHexState = Qt::Checked;
    else if(arg1 == Qt::Unchecked)
        ReceWindowHexState = Qt::Unchecked;
}

void MainWindow::on_checkBoxSendHex_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked)
        SendWindowHexState = Qt::Checked;
    else if(arg1 == Qt::Unchecked)
        SendWindowHexState = Qt::Unchecked;
}

void MainWindow::on_pushButtonRecvClear_clicked()
{
    ui->textEditRecv->clear();
}

void MainWindow::on_pushButtonSendClear_clicked()
{
    ui->textEditSend->clear();
}

void MainWindow::on_pushButtonSend_clicked()
{
    if(ui->textEditSend->toPlainText() != NULL)
    {
        if(SendWindowHexState == Qt::Unchecked)
        {
            QString str;
            str = ui->textEditSend->toPlainText();
            if (str[str.length()-1] == '\n')
            {
                str[str.length()-1] = '\r';
                str += "\n";
            }
            PCSerialPort.write(str.toLatin1());
        }
        else if(SendWindowHexState == Qt::Checked)
        {
            QString str;
            str = ui->textEditSend->toPlainText();
            QByteArray sendData;
            stringToHex(str, sendData);
            PCSerialPort.write(sendData);
        }
    }
}

void MainWindow::stringToHex(QString str, QByteArray &sendData)
{
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    sendData.resize(len/2);
    for(int i=0; i<len; )
    {
        char lstr;
        char hstr;
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = convertHexChar(hstr);
        lowhexdata = convertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16 + lowhexdata;
        i++;
        sendData[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    sendData.resize(hexdatalen);
}

char MainWindow::convertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else
        return (-1);
}

void MainWindow::on_pushButtonGetPara_clicked()
{
    QString str;
    str = "ATI\r\n";
    QTimer::singleShot(0, this, [this, str] () {
        sendOneCmd(str);
    });

    str = "ATE\r\n";
    QTimer::singleShot(2*1000, this, [this, str] () {
        sendOneCmd(str);
    });

    str = "AT+CPIN?\r\n";
    QTimer::singleShot(4*1000, this, [this, str] () {
        sendOneCmd(str);
    });

    str = "AT+QCCID\r\n";
    QTimer::singleShot(6*1000, this, [this, str] () {
        sendOneCmd(str);
    });

    str = "AT+CIMI\r\n";
    QTimer::singleShot(8*1000, this, [this, str] () {
        sendOneCmd(str);
    });

    str = "AT+COPS?\r\n";
    QTimer::singleShot(10*1000, this, [this, str] () {
        sendOneCmd(str);
    });

    str = "AT+QISTATE\r\n";
    QTimer::singleShot(12*1000, this, [this, str] () {
        sendOneCmd(str);
    });
}

void MainWindow::on_pushButtonEnterATMode_clicked()
{
    QString str;
    str = "+++";
    QTimer::singleShot(0, this, [this, str] () {
        sendOneCmd(str);
    });

    str = "+++";
    QTimer::singleShot(1000, this, [this, str] () {
        sendOneCmd(str);
    });
}

void MainWindow::on_pushButtonExitATMode_clicked()
{
    QString str;
    str = "AT+ENTM\r\n";
    QTimer::singleShot(0, this, [this, str] () {
        sendOneCmd(str);
    });
}
