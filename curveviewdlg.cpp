#include "curveviewdlg.h"
#include "ui_curveviewdlg.h"
#include <QScrollBar>

CurveViewDlg::CurveViewDlg(DataModel * model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CurveViewDlg)
{
    ui->setupUi(this);
	cv = new CurveView();
	ui->scrollArea->setWidget(cv);
	cv->set_model(model);
	cv->set_column(1);
	cv->set_height(150);
	cv->set_width(600);
	connect(ui->scroll_utc, &QScrollBar::valueChanged, this, &CurveViewDlg::on_new_utc);
	view_duration = 8;
	play_timer = -1;
	display_end_utc = 0;
	display_start_utc = 0;
	display_idx_utc = 0;
}

CurveViewDlg::~CurveViewDlg()
{
    delete ui;
}

void CurveViewDlg::add_cureve(int channel, DATA_TYPE type)
{
	CurveConfig cc;
	cc.channel = channel;
	cc.type = type;
	cc.hspacer = 5;
	cc.vspacer = 5;
	cc.axis_color = Qt::blue;
	cc.bg_color = Qt::white;
	cc.word_color = Qt::blue;
	cc.curve_color = Qt::red;
	cc.scale = 1;
	cc.y_shift = 0;
	cv->add_cureve(cc);
	cv->setGeometry(20, 20, cv->width(), cv->height());
	update_scroll_bar();
}

void CurveViewDlg::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == play_timer) {
		display_end_utc = cv->get_model()->get_current_utc();
		if (display_end_utc != UTC_END) {
			display_start_utc = cv->get_model()->get_original_utc();
			display_idx_utc = display_end_utc - view_duration * UTC_UNIT;
			display_idx_utc = max(0LL, display_idx_utc);
			update_scroll_bar();
		}
		else
			display_end_utc = 0;
	}
}

void CurveViewDlg::on_play_button_clicked()
{
	play_timer = startTimer(1000);
}

void CurveViewDlg::on_pause_button_clicked()
{
	killTimer(play_timer);
	play_timer = -1;
}

void CurveViewDlg::on_zoomin_clicked()
{
	if (view_duration > 1) {
		view_duration = view_duration / 2;
		if (play_timer == -1) 
			display_idx_utc = display_idx_utc + view_duration * UTC_UNIT;
		else 
			display_idx_utc = display_end_utc - view_duration * UTC_UNIT;
		display_idx_utc = max(0LL, display_idx_utc);
		update_scroll_bar();
	}
}

void CurveViewDlg::on_zoomout_clicked()
{
	if (view_duration < 32768 && view_duration < cv->get_model()->get_duration() / UTC_UNIT) {
		view_duration = view_duration * 2;
		if (play_timer == -1)
			display_idx_utc = display_idx_utc - view_duration * UTC_UNIT / 2;
		else
			display_idx_utc = display_end_utc - view_duration * UTC_UNIT;
		display_idx_utc = max(0LL, display_idx_utc);
		update_scroll_bar();
	}
}

void CurveViewDlg::on_new_utc(int value)
{
	uint64_t start = max(0, value);
	start = start * UTC_UNIT + display_start_utc;
	uint64_t end = start + ui->scroll_utc->pageStep() * UTC_UNIT;
	cv->set_utc(start, end);
}

void CurveViewDlg::update_scroll_bar()
{
	ui->scroll_utc->setMinimum(0);
	int maximum = (display_end_utc - display_start_utc) / UTC_UNIT - view_duration;
	ui->scroll_utc->setMaximum(max(maximum, 0));
	ui->scroll_utc->setPageStep(view_duration);
	ui->scroll_utc->setSingleStep(max(1, (int) (view_duration / 3)));
	int value = (display_idx_utc - display_start_utc) / UTC_UNIT;
	ui->scroll_utc->setValue(max(value, 0));
	if (value <= 0) //when value =0, on_new_utc won't be called
		on_new_utc(0);
}