#include "datasource.h"
#include <QMessageBox>

DataSource::DataSource(QObject *parent) : QObject(parent)
{
    connect_state = NO_CONNECT;
	connect(&serial, SIGNAL(readyRead()), this, SLOT(read_data()));
}

int DataSource::connect_serial(SettingsDialog::Settings p)
{
    if (connect_state == CONNECT_SERIAL || connect_state == NO_CONNECT) {
        disconnect_serial();
        serial.setPortName(p.name);
        serial.setBaudRate(p.baudRate);
        serial.setDataBits(p.dataBits);
        serial.setParity(p.parity);
        serial.setStopBits(p.stopBits);
        serial.setFlowControl(p.flowControl);
        if (serial.open(QIODevice::ReadWrite)) {
            qInfo("Connected to %s, rate=%d, parity=%d, stopbit=%d, flow_ctrl=%d", p.name.toStdString().c_str(),
                  p.baudRate, p.parity, p.stopBits, p.flowControl);
            connect_state = CONNECT_SERIAL;
            return 1;
        } else {
            connect_state = NO_CONNECT;
            return 0;
        }
    }
}

void DataSource::disconnect_serial()
{
    if (serial.isOpen())
        serial.close();
    if (connect_state == CONNECT_SERIAL)
        connect_state = NO_CONNECT;
}

ConnectStatus DataSource::get_connect_status()
{
	return connect_state;
}

void DataSource::read_data()
{
    QByteArray data;
    if (connect_state == CONNECT_SERIAL)
        data = serial.readAll();
    buf.append(data);
    bool finish = false;
    while (!finish) {
        finish = true;
        for (int i=0; i<buf.size(); i++)
        if (buf[i] == '\r') {
            emit data_recv(buf.left(i));
            finish = false;
            buf.remove(0, i + 1);
            break;
        }
    }
}

void DataSource::data_send(const QByteArray &data)
{
	if (connect_state == CONNECT_SERIAL) {
		serial.write(data);
		char c = 13;
		serial.write(&c);
	}
}
