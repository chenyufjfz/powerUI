#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include "settingsdialog.h"

#define LOG_CHAR '`'
enum ConnectStatus {
    NO_CONNECT,
    CONNECT_SERIAL
};
class DataSource : public QObject
{
    Q_OBJECT
public:
    explicit DataSource(QObject *parent = 0);
    //return 1 means success, 0 means fail
    int connect_serial(SettingsDialog::Settings p);
    void disconnect_serial();    
	ConnectStatus get_connect_status();

signals:
    void data_recv(const QByteArray &data);

public slots:
    void data_send(const QByteArray &data);
	void read_data();

protected:
    QByteArray buf;
    QSerialPort serial;
    ConnectStatus connect_state;
};

#endif // DATASOURCE_H
