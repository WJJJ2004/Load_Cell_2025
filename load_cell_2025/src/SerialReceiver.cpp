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

#include "../include/load_cell_2025/SerialReceiver.hpp"
#include <QDebug>
#include <QStringList>

SerialReceiver::SerialReceiver(QObject* parent)
    : QObject(parent), serial(new QSerialPort(this))
{
    connect(serial, &QSerialPort::readyRead, this, &SerialReceiver::readData);
}

SerialReceiver::~SerialReceiver()
{
    closePort();
}

bool SerialReceiver::openPort(const QString& portName, int baudRate)
{
    serial->setPortName(portName);
    serial->setBaudRate(baudRate);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (!serial->open(QIODevice::ReadOnly)) {               // 안된다면? !serial -> open(QIODevice::ReadWrite) 으로 수정해보고 시도 할 것
        qWarning() << "Failed to open port:" << portName;
        return false;
    }
    return true;
}

void SerialReceiver::closePort()
{
    if (serial->isOpen())
        serial->close();
}

bool SerialReceiver::isOpen() const
{
    return serial->isOpen();
}

void SerialReceiver::readData()
{
    buffer.append(serial->readAll()); // 버퍼에 문자열 푸쉬백
    while (true)
    {
        int newlineIndex = buffer.indexOf('\n');
        if (newlineIndex == -1) break; // 버퍼에 \n 문자를 찾지 못함 >> -1 반환

        QByteArray line = buffer.left(newlineIndex).trimmed(); // 버퍼 바이트 배열에서 양쪽 공백 문자 제거
        buffer.remove(0, newlineIndex + 1); // pos , lenth

        std::vector<int16_t> r_lc, l_lc;
        if (parseFrame(line, r_lc, l_lc))
        {
            emit dataReceived(r_lc, l_lc);
        }
    }
}

bool SerialReceiver::parseFrame(const QByteArray& line, std::vector<int16_t>& r_lc, std::vector<int16_t>& l_lc)
{
    if (!line.contains("LOADCELL"))
    {
        std::cout << "\033[33m"
            << "[SR] token input: \"" << line.toStdString() << "\""
        << "\033[0m" << std::endl;

        return false;
    }

    QString dataStr = QString::fromUtf8(line);
    QStringList tokens = dataStr.split(' ', Qt::SkipEmptyParts);
    if (tokens.size() < 9) return false;

    for (int i = 1; i <= 8; ++i)
    {
        bool ok = false;
        int value = tokens[i].toInt(&ok);
        if (!ok) return false;

        if (i <= 4) r_lc.push_back(static_cast<int16_t>(value));
        else l_lc.push_back(static_cast<int16_t>(value));
    }
    return true;
}
