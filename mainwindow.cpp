#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "curveviewdlg.h"
#include <QSplitter>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	tbl_view.setModel(&model);
	CurveViewDlg * cv = new CurveViewDlg(&model);
	QSplitter *splitter = new QSplitter(Qt::Horizontal);
	QSplitter *vsplitter = new QSplitter(Qt::Vertical);
	cv->add_cureve(0, VoltageAmp);
	cv->add_cureve(0, VoltageDeg);
	cv->add_cureve(0, CurrentAmp);
	cv->add_cureve(0, CurrentDeg);
	vsplitter->addWidget(&tbl_view);
	vsplitter->addWidget(&console);
    vsplitter->setStretchFactor(0, 2);
	vsplitter->setStretchFactor(1, 1);

	splitter->addWidget(vsplitter);
	splitter->addWidget(cv);
	splitter->setStretchFactor(0, 1);
	splitter->setStretchFactor(1, 1);
	setCentralWidget(splitter);
    connect(&data_source, &DataSource::data_recv, &console, &Console::put_data);    
    connect(&console, &Console::command_ready, &data_source, &DataSource::data_send);
	connect(&data_source, &DataSource::data_recv, &model, &DataModel::put_vi_data);
    resize(1300, 750);

    status_label = new QLabel();
    status_label->setMaximumSize(500, 20);
    status_label->setText("Disconnected");
    ui->statusBar->addWidget(status_label);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_actionConnect_triggered()
{
    SettingsDialog sdlg;
	if (sdlg.exec() == QDialog::Accepted) {
        int connected = data_source.connect_serial(sdlg.settings());
        if (connected)
            status_label->setText("Connected");
	}	
}

void MainWindow::on_actionDisconnect_triggered()
{
	data_source.disconnect_serial();
    status_label->setText("Disconnected");
}

void MainWindow::on_actionSave_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save File"),
        "",
        tr("Database (*.txt)"));
    if (!filename.isEmpty()) {
        qInfo("UI: save as %s", filename.toStdString().c_str());
        model.write_file(filename.toStdString());
    }
}
