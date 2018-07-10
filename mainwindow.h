#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QTime>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int ReceWindowHexState = Qt::Unchecked;
    int SendWindowHexState = Qt::Unchecked;

    void stringToHex(QString str, QByteArray &sendData);

    char convertHexChar(char ch);

private slots:
    void readData();

    void on_checkBoxRecvHex_stateChanged(int arg1);

    void on_checkBoxSendHex_stateChanged(int arg1);

    void on_pushButtonRecvClear_clicked();

    void on_pushButtonSendClear_clicked();

    void on_pushButtonSend_clicked();

    void on_pushButtonGetPara_clicked();

    void on_pushButtonEnterATMode_clicked();

    void on_pushButtonOpenPort_clicked();

    void sendOneCmd(QString str);


    void on_pushButtonExitATMode_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort PCSerialPort;
};

#endif // MAINWINDOW_H
