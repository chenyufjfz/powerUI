#include "console.h"
#include "ui_console.h"
#include "datasource.h"
#include <QScrollBar>

Console::Console(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Console)
{
    ui->setupUi(this);
    connect(ui->console_input, SIGNAL(returnPressed()), this, SLOT(get_data()));
}

void Console::put_data(const QByteArray &data)
{
    if (data[0] == LOG_CHAR)
        return;

    ui->console_output->insertPlainText(QString(data));
    QScrollBar *bar = ui->console_output->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::get_data()
{
	QByteArray data = ui->console_input->text().toLocal8Bit();
    emit command_ready(data);
	ui->console_input->clear();
}

Console::~Console()
{
    delete ui;
}
