#ifndef CURVEVIEWDLG_H
#define CURVEVIEWDLG_H

#include <QDialog>
#include "curveview.h"

namespace Ui {
class CurveViewDlg;
}

class CurveViewDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CurveViewDlg(DataModel * model, QWidget *parent = 0);
    ~CurveViewDlg();
	void add_cureve(int channel, DATA_TYPE type);

protected:
	void timerEvent(QTimerEvent *event);

private slots:
    void on_play_button_clicked();

    void on_pause_button_clicked();

    void on_zoomin_clicked();

    void on_zoomout_clicked();

	void on_new_utc(int value);

private:
	CurveView * cv;
    Ui::CurveViewDlg *ui;
	int play_timer;
	int64_t display_start_utc, display_end_utc, display_idx_utc;
	double view_duration;
	void update_scroll_bar();
};

#endif // CURVEVIEWDLG_H
