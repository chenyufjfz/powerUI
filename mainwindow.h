#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "console.h"
#include "settingsdialog.h"
#include "datasource.h"
#include "datamodel.h"
#include <QTableView>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionConnect_triggered();

    void on_actionDisconnect_triggered();

    void on_actionSave_triggered();

private:
    QLabel *status_label;
    Console console;
    DataSource data_source;
	DataModel model;
	QTableView tbl_view;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
