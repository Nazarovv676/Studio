#ifndef DONUTBREAKDOWNCHART_H
#define DONUTBREAKDOWNCHART_H

#include <QtCharts/QChart>
#include <QtCharts/QPieSeries>

QT_CHARTS_USE_NAMESPACE

class DonutBreakdownChart : public QChart
{
public:
    DonutBreakdownChart(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
    void addBreakdownSeries(QPieSeries *series, QColor color);

    QStringList getMarkersList() const;

    void setMarkersList(const QStringList &value);

private:
    void recalculateAngles();
    void updateLegendMarkers();

private:
    QPieSeries *m_mainSeries;
};

#endif // DONUTBREAKDOWNCHART_H
