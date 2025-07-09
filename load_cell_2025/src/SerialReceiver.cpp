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

    if (!serial->open(QIODevice::ReadOnly)) {
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
    buffer.append(serial->readAll());
    while (true)
    {
        int newlineIndex = buffer.indexOf('\n');
        if (newlineIndex == -1) break;

        QByteArray line = buffer.left(newlineIndex).trimmed();
        buffer.remove(0, newlineIndex + 1);

        std::vector<int16_t> r_lc, l_lc;
        if (parseFrame(line, r_lc, l_lc))
        {
            emit dataReceived(r_lc, l_lc);
        }
    }
}

bool SerialReceiver::parseFrame(const QByteArray& line, std::vector<int16_t>& r_lc, std::vector<int16_t>& l_lc) 
{
    if (!line.contains("LOADCELL")) return false;

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
