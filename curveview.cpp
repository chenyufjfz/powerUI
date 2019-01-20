#include "curveview.h"

CurveView::CurveView(QWidget *parent) : QWidget(parent)
{
	setAttribute(Qt::WA_OpaquePaintEvent, true);
	QPalette palette(this->palette());
	palette.setColor(QPalette::Background, Qt::gray);
	this->setPalette(palette);
	setAutoFillBackground(false);
	setAttribute(Qt::WA_NoSystemBackground, true);
	start = 0;
	end = 0;
	chigh = 120;
	cwide = 500;
	column = 1;
	resize(cwide * column, 900);
}

void CurveView::add_cureve(CurveConfig cc)
{
	ccs.push_back(cc);
	if (end != 0)
		update();
}

void CurveView::set_column(int _colume)
{
	column = _colume;
	resize(cwide * column, chigh * (((int) ccs.size() - 1) / column + 1));
	if (end != 0)
		update();
}

void CurveView::set_height(int _height)
{
	chigh = _height;
	resize(cwide * column, chigh * (((int) ccs.size() - 1) / column + 1));
	if (end != 0)
		update();
}

void CurveView::set_width(int _width)
{
	cwide = _width;
	resize(cwide * column, chigh * (((int) ccs.size() - 1) / column + 1));
	if (end != 0)
		update();
}

void CurveView::set_model(DataModel * _model)
{
	model = _model;
	if (end != 0)
		update();
}

DataModel * CurveView::get_model()
{
	return model;
}

void CurveView::set_utc(uint64_t _start, uint64_t _end)
{
	start = _start;
	end = _end;
	update();
}

void CurveView::paintEvent(QPaintEvent *)
{
	QImage image(size(), QImage::Format_RGB32);
	image.fill(QColor(240, 240, 240));	

	if (end == 0 || ccs.size() == 0) {
		QPainter paint(this);
		paint.drawImage(QPoint(0, 0), image);
		return;
	}

	QPainter painter(&image);		
	int hspacer = 10, vspacer = 10;
	int w = width() / column - hspacer;
	int h = min(chigh, height() / (((int)ccs.size() - 1) /column + 1)) - vspacer;
	
	for (int i = 0; i < (int) ccs.size(); i++) {
		ccs[i].start = start;
		ccs[i].end = end;
		ccs[i].r.setTopLeft(QPoint(i % column * (w + hspacer) + hspacer / 2, i / column * (h + vspacer) + vspacer / 2));
		ccs[i].r.setWidth(w);
		ccs[i].r.setHeight(h);
		painter.drawImage(ccs[i].r.topLeft(), model->draw_curve(ccs[i]));
	}
	QPainter paint(this);
	paint.drawImage(QPoint(0, 0), image);
}