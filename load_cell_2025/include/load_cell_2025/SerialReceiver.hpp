// ***************** Serial Packit Format ***************
//
// LOADCELL <R0> <R1> <R2> <R3> <L0> <L1> <L2> <L3>\n
//
// ******************************************************
//
// ******************* Example Packit ********************
//
// LOADCELL 1234 1240 1300 1280 1400 1380 1320 1390\n
//
// ******************************************************

#ifndef SERIAL_RECEIVER_HPP
#define SERIAL_RECEIVER_HPP

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <vector>
#include <iostream>

class SerialReceiver : public QObject
{
    Q_OBJECT

public:
    explicit SerialReceiver(QObject* parent = nullptr);
    ~SerialReceiver();

    bool openPort(const QString& portName, int baudRate = QSerialPort::Baud115200);
    void closePort();
    bool isOpen() const;

signals:
    void dataReceived(const std::vector<int16_t>& r_lc, const std::vector<int16_t>& l_lc);

private slots:
    void readData();

private:
    QSerialPort* serial;
    QByteArray buffer;

    bool parseFrame(const QByteArray& line, std::vector<int16_t>& r_lc, std::vector<int16_t>& l_lc);
};

#endif
