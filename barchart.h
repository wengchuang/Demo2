#ifndef BARCHART_H
#define BARCHART_H

#include <qwt_plot.h>
#include <QStringList>



class DistroChartItem;

class Barchart : public QwtPlot
{
    Q_OBJECT

public:
    explicit Barchart(QWidget *parent = 0);
    void     repaintChart(const QString& title,QVector<double>& samples);

signals:

public slots:
    void  exportChart();
private:
    void populate();
    DistroChartItem* item;
    QStringList   mDistros;

};

#endif // Barchart_H
