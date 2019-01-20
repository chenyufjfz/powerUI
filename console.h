#ifndef CONSOLE_H
#define CONSOLE_H

#include <QDialog>

namespace Ui {
class Console;
}

class Console : public QDialog
{
    Q_OBJECT

signals:
    void command_ready(const QByteArray &data);

public:
    explicit Console(QWidget *parent = 0);
    ~Console();

public slots:
    void put_data(const QByteArray &data);
	void get_data();

private:    
    Ui::Console *ui;
};

#endif // CONSOLE_H
