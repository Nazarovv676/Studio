#include "donutbreakdownchart.h"
#include "mainslice.h"
#include <QtCharts/QPieSlice>
#include <QtCharts/QPieLegendMarker>
#include <QDebug>

QT_CHARTS_USE_NAMESPACE

DonutBreakdownChart::DonutBreakdownChart(QGraphicsItem *parent, Qt::WindowFlags wFlags)
    : QChart(QChart::ChartTypeCartesian, parent, wFlags)
{
    m_mainSeries = new QPieSeries();
    m_mainSeries->setPieSize(0.7);
    QChart::addSeries(m_mainSeries);
}

void DonutBreakdownChart::addBreakdownSeries(QPieSeries *breakdownSeries, QColor color)
{
    QFont font("Arial", 8);

    MainSlice *mainSlice = new MainSlice(breakdownSeries);
    mainSlice->setName(breakdownSeries->name());
    mainSlice->setValue(breakdownSeries->sum());
    m_mainSeries->append(mainSlice);

    mainSlice->setBrush(color);
    mainSlice->setLabelVisible();
    mainSlice->setLabelColor(Qt::white);
    mainSlice->setLabelPosition(QPieSlice::LabelInsideHorizontal);
    mainSlice->setLabelFont(font);

    breakdownSeries->setPieSize(0.8);
    breakdownSeries->setHoleSize(0.7);
    breakdownSeries->setLabelsVisible();
    const auto slices = breakdownSeries->slices();
    for (QPieSlice *slice : slices) {
        color = color.lighter(115);
        slice->setBrush(color);
        slice->setLabelFont(font);
    }

    QChart::addSeries(breakdownSeries);

    recalculateAngles();

    updateLegendMarkers();
}

void DonutBreakdownChart::recalculateAngles()
{
    qreal angle = 0;
    const auto slices = m_mainSeries->slices();
    for (QPieSlice *slice : slices)
    {
        QPieSeries *breakdownSeries = qobject_cast<MainSlice *>(slice)->breakdownSeries();
        breakdownSeries->setPieStartAngle(angle);
        angle += slice->percentage() * 360.0;
        breakdownSeries->setPieEndAngle(angle);
    }
}

void DonutBreakdownChart::updateLegendMarkers()
{
    const auto allseries = series();
    for (QAbstractSeries *series : allseries)
    {
        const auto markers = legend()->markers(series);
        for (QLegendMarker *marker : markers)
        {
            QPieLegendMarker *pieMarker = qobject_cast<QPieLegendMarker *>(marker);
            if (series == m_mainSeries) {
                pieMarker->setVisible(false);
            }
            else
            {
                pieMarker->setLabel(QString("%1 %2%")
                                    .arg(pieMarker->slice()->label())
                                    .arg(pieMarker->slice()->percentage() * 100, 0, 'f', 2));
                pieMarker->setFont(QFont("Nunito", 9));
            }
        }
    }
}
