#include "datamodel.h"
#include <algorithm>
#include <math.h>

#define PI 3.1415926536

float VIBuf::get_data(int channel, DATA_TYPE type) const
{
	switch (type) {
	case VoltageAmp:
		return vi[channel * 2][0] / 50.0;
	case VoltageDeg:
		return vi[channel * 2][1] / 100.0;
	case CurrentAmp:
		return vi[channel * 2 + 1][0] / 5000.0;
	case CurrentDeg:
		return vi[channel * 2 + 1][1] / 100.0;
	case AdmittanceAmp:
		return (vi[channel * 2][0] != 0) ?
			0.01 * vi[channel * 2 + 1][0] / vi[channel * 2][0] :
			INFINITY;
	case AdmittanceDeg:
		return (vi[channel * 2][0] != 0) ?
			((int) vi[channel * 2 + 1][1] - vi[channel * 2][1]) / 100.0 : 0;
	case ImpedanceAmp:
		return (vi[channel * 2 + 1][0] != 0) ?
			100.0 * vi[channel * 2][0] / vi[channel * 2 + 1][0] :
			INFINITY;
	case ImpedanceDeg:
		return (vi[channel * 2 + 1][0] != 0) ? 
			((int) vi[channel * 2][1] - vi[channel * 2 + 1][1]) / 100.0 : 0;
	case PowerActive:
		return vi[channel * 2][0] / 250000.0 * vi[channel * 2 + 1][0] * 
			cos(((int)vi[channel * 2][1] - vi[channel * 2 + 1][1]) / 100.0 * PI / 180);
	case PowerReactive:
		return vi[channel * 2][0] / 250000.0 * vi[channel * 2 + 1][0] *
			sin(((int)vi[channel * 2][1] - vi[channel * 2 + 1][1]) / 100.0 * PI / 180);
	}
}

DataModel::DataModel()
{

}

int DataModel::rowCount(const QModelIndex &) const
{
    return (int) vidb.size();
}

int DataModel::columnCount(const QModelIndex &) const
{
    return 9;
}

QVariant DataModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= vidb.size() || index.row() < 0 || index.column() >= 9)
		return QVariant();

	if (role == Qt::DisplayRole) {
		char a[20];
		if (index.column() == 0)
			sprintf(a, "%lld", vidb[index.row()].utc);
		else {
			int channel = (index.column() - 1) / 2;
			DATA_TYPE type1 = (index.column() % 2) ? VoltageAmp : CurrentAmp;
			DATA_TYPE type2 = (index.column() % 2) ? VoltageDeg : CurrentDeg;
			sprintf(a, "%5f,%5f", vidb[index.row()].get_data(channel, type1), vidb[index.row()].get_data(channel, type2));
		}
		return QString::fromLocal8Bit(a);
	}
	return QVariant();
}

QVariant DataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	if (orientation == Qt::Horizontal) {
		switch (section) {
		case 0:
			return tr("UTC");
		case 1:
			return tr("V1");
		case 2:
			return tr("I1");
		case 3:
			return tr("V2");
		case 4:
			return tr("I2");
		case 5:
			return tr("V3");
		case 6:
			return tr("I3");
		case 7:
			return tr("V4");
		case 8:
			return tr("I4");
		default:
			return QVariant();
		}
	}
	return QVariant();
}

/*
Input start, utc start time, =0 means from begining
Input end, utc end time, = 0xffffffffffffffff means to ending
Input channel, 0..3 channel
Input type, U, I, Y, P
Input number, output vector with at most number size
Output d
*/
void DataModel::get_data(uint64_t start, uint64_t end, int channel, DATA_TYPE type, int number, vector<pair<uint64_t, float> > & d) const
{
	d.clear();
	if (vidb.empty())
		return;
	if (start != 0) {
		VIBuf f;
		f.utc = start;
		start = lower_bound(vidb.begin(), vidb.end(), f) - vidb.begin();
		start = min(start, vidb.size() - 1);
	}
	if (end != UTC_END) {
		VIBuf f;
		f.utc = end;
		end = lower_bound(vidb.begin(), vidb.end(), f) - vidb.begin();
		end = min(end, vidb.size() - 1);
	}
	else
		end = vidb.size() - 1;
	if (start > end)
		return;
	if (start == end) {
		d.push_back(make_pair(vidb.back().utc, vidb.back().get_data(channel, type)));
		return;
	}
	double t = vidb[start].utc;
	double step = (vidb[end].utc - t) / (number - 1);
	for (int i = 0, j = start; i < number; i++, t+=step) {
		while (j < end && vidb[j + 1].utc < t)
			j++;
		if (j==end || t < (vidb[j + 1].utc + vidb[j].utc) / 2) {
			if (d.empty() || vidb[j].utc != d.back().first)
				d.push_back(make_pair(vidb[j].utc, vidb[j].get_data(channel, type)));
		}
		else {
			if (d.empty() || vidb[j + 1].utc != d.back().first)
				d.push_back(make_pair(vidb[j + 1].utc, vidb[j + 1].get_data(channel, type)));
		}
	}
}

QImage DataModel::draw_curve(CurveConfig c)  const
{
	char sh[100];
	QImage image(c.r.size(), QImage::Format_RGB32);
	image.fill(c.bg_color);
	QPainter painter(&image);	
	int height = c.r.height();
	int width = c.r.width();

	//1 draw axis
	painter.setPen(QPen(c.axis_color, 1));
	painter.drawLine(c.hspacer, 5, c.hspacer, height - 5); // y axis
	painter.setPen(QPen(c.axis_color, 1, Qt::DotLine));
	painter.drawLine(c.hspacer, height - c.vspacer, width - c.hspacer, height - c.vspacer);
	painter.drawLine(c.hspacer, height / 2, width - c.hspacer, height / 2);
	painter.drawLine(c.hspacer, c.vspacer, width - c.hspacer, c.vspacer);

	//2 draw word
	float up_bound, down_bound;
	switch (c.type) {
	case VoltageAmp:
		up_bound = 300;
		down_bound = 0;
		break;
	case CurrentAmp:
		up_bound = 3;
		down_bound = 0;
		break;
	case AdmittanceAmp:
		up_bound = 0.01;
		down_bound = 0;
		break;
	case ImpedanceAmp:
		up_bound = 100;
		down_bound = 0;
		break;
	case VoltageDeg:
	case CurrentDeg:
	case AdmittanceDeg:
	case ImpedanceDeg:
		up_bound = 180;
		down_bound = -180;
		break;
	case PowerActive:
	case PowerReactive:
		up_bound = 500;
		down_bound = 0;
		break;
	}
	up_bound = up_bound * c.scale;
	down_bound = down_bound * c.scale;
	float shift = (up_bound - down_bound) * c.y_shift;
	up_bound += shift;
	down_bound += shift;
	painter.setPen(c.word_color);
	sprintf(sh, "%5.1f", up_bound);
    painter.drawText(QPoint(c.hspacer + 1, c.vspacer + 10), QString::fromLocal8Bit(sh));
	sprintf(sh, "%5.1f", down_bound);
	painter.drawText(QPoint(c.hspacer + 1, height - c.vspacer), QString::fromLocal8Bit(sh));
	sprintf(sh, "%5.1f", (up_bound + down_bound) / 2);
	painter.drawText(QPoint(c.hspacer + 1, height / 2), QString::fromLocal8Bit(sh));

	//3 draw curve
	painter.setPen(QPen(c.curve_color, 1));
	vector<pair<uint64_t, float> > d;
	get_data(c.start, c.end, c.channel, c.type, width / 2, d);
	if (d.empty())
		return image;
	if (c.start == 0)
		c.start = d[0].first;
	if (c.end == UTC_END)
		c.end = d.back().first;
	float slopey = (c.vspacer * 2 - height) / (up_bound - down_bound);
#define Value2Y(v) (slopey * ((v) - down_bound) + height - c.vspacer)
	for (int i = 0; i < (int)d.size(); i++)
		d[i].second = Value2Y(d[i].second);
	double slopex = ((double) c.end - c.start) / (width - c.hspacer * 2);
#define X2UTC(x) (slopex * ((x) - c.hspacer) + c.start)
#define InterPol(t0, v0, t1, v1, t) ((v1 - v0) / (t1 - t0) * ((t) - t0) + v0)
	int prev_y = 0x80000000;
	for (int x = c.hspacer + 1, i=0; x <= width - c.hspacer; x += 2) {
		double t = X2UTC(x);
		while (i + 1 < d.size() && d[i + 1].first < t)
			i++;
		if (i + 1 == d.size() || d[i].first > t) {
			prev_y = 0x80000000;
			continue;
		}
		int y = InterPol(d[i].first, d[i].second, d[i + 1].first, d[i + 1].second, t);
		if (prev_y != 0x80000000)
			painter.drawLine(x - 2, prev_y, x, y);
		prev_y = y;
	}

	return image;
}

uint64_t DataModel::get_current_utc() const
{
	if (vidb.empty())
		return UTC_END;
	return vidb.back().utc;
}

uint64_t DataModel::get_original_utc() const
{
	if (vidb.empty())
		return UTC_END;
	return vidb[0].utc;
}

uint64_t DataModel::get_duration() const
{
	return vidb.back().utc - vidb[0].utc;
}

void DataModel::write_file(string file_name)
{
	FILE * fp = fopen(file_name.c_str(), "wt");
	for (unsigned i = 0; i < vidb.size(); i++)
		fprintf(fp, "%lld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		vidb[i].utc, vidb[i].vi[0][0], vidb[i].vi[0][1], vidb[i].vi[1][0], vidb[i].vi[1][1],
		vidb[i].vi[2][0], vidb[i].vi[2][1], vidb[i].vi[3][0], vidb[i].vi[3][1],
		vidb[i].vi[4][0], vidb[i].vi[4][1], vidb[i].vi[5][0], vidb[i].vi[5][1],
		vidb[i].vi[6][0], vidb[i].vi[6][1], vidb[i].vi[7][0], vidb[i].vi[7][1]);
	fclose(fp);
}

void DataModel::read_file(string file_name)
{

}

void DataModel::put_vi_data(const QByteArray &data)
{
    if (data[0] != '`' || data[1] != '1')
        return;
    VIBuf vi;
	if (data[2] != 'V' || data[3] != 'I') {
		qInfo(data);
		return;
	}
	char sh[200];
	for (int i = 0; i < data.size(); i++) {
		if (data[i] == '\n')
			sh[i] = 0;
		else
			sh[i] = data[i];
	}
	sscanf(sh,
           "`1VI,%llx,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",
           &vi.utc, &vi.vi[0][0], &vi.vi[0][1], &vi.vi[1][0], &vi.vi[1][1],
            &vi.vi[2][0], &vi.vi[2][1], &vi.vi[3][0], &vi.vi[3][1],
            &vi.vi[4][0], &vi.vi[4][1], &vi.vi[5][0], &vi.vi[5][1],
            &vi.vi[6][0], &vi.vi[6][1], &vi.vi[7][0], &vi.vi[7][1]);

	if (!vidb.empty()) {
        beginResetModel();
		while (!vidb.empty() && vidb.back().utc >= vi.utc)
			vidb.pop_back();
        endResetModel();
	}
	beginInsertRows(QModelIndex(), (int) vidb.size(), (int) vidb.size());
    vidb.push_back(vi);
	endInsertRows();
}
