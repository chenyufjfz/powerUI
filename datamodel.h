#ifndef DATAMODEL_H
#define DATAMODEL_H
#include <QAbstractTableModel>
#include <stdint.h>
#include <vector>
#include <QPainter>
#include <string>

using namespace std;

#define CHANNEL 8
#define UTC_END 0xffffffffffffffff
#define UTC_UNIT 1000

enum DATA_TYPE {
    VoltageAmp,
    VoltageDeg,
    CurrentAmp,
    CurrentDeg,
    AdmittanceAmp,
    AdmittanceDeg,
    ImpedanceAmp,
    ImpedanceDeg,
    PowerActive,
    PowerReactive
};

struct VIBuf {
    uint64_t utc;
    int16_t vi[CHANNEL][2];
	bool operator < (const VIBuf & a) const { 
		return utc < a.utc;
	}
	float get_data(int channel, DATA_TYPE type) const;
};

struct VIBuf_comp {
    bool operator() (const VIBuf &a1,const VIBuf &a2) {
        return a1.utc < a2.utc;
    }
};

struct CurveConfig {
	int channel;
	DATA_TYPE type;
	uint64_t start, end;
	QColor curve_color, axis_color, word_color, bg_color;
	double scale, y_shift;
	int hspacer, vspacer;
	QRect r;
};

class DataModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    DataModel();
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
	void get_data(uint64_t start, uint64_t end, int channel, DATA_TYPE type, int number, vector<pair<uint64_t, float> > & d) const;
	QImage draw_curve(CurveConfig c) const;
	uint64_t get_current_utc() const;
	uint64_t get_original_utc() const;
	uint64_t get_duration() const;
	void write_file(string file_name);
	void read_file(string file_name);

public slots:
    void put_vi_data(const QByteArray &data);

protected:
    vector<VIBuf> vidb;
};

#endif // DATAMODEL_H
