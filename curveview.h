#ifndef CURVEVIEW_H
#define CURVEVIEW_H

#include <QWidget>
#include "datamodel.h"

class CurveView : public QWidget
{
    Q_OBJECT
public:
    explicit CurveView(QWidget *parent = 0);
	void add_cureve(CurveConfig cc);
	void set_column(int _colume);
	void set_height(int _height);
	void set_width(int _width);
	void set_model(DataModel * _model);
	DataModel * get_model();
	//use set_utc to trigger repaint
	void set_utc(uint64_t _start, uint64_t _end);
signals:

public slots:

protected:
	void paintEvent(QPaintEvent *e);
	vector<CurveConfig> ccs;
	int column;
	int chigh, cwide;
	DataModel * model;
	uint64_t start, end;
};

#endif // CURVEVIEW_H
