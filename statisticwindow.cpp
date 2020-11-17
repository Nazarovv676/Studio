#include "statisticwindow.h"
#include "ui_statisticwindow.h"

#include "Data/dbconnection.h"
#include "Data/log.h"
#include "DonutBreakDownChart/donutbreakdownchart.h"

#include <QDebug>
#include <exception>
#include <QMessageBox>
#include <QtAlgorithms>

StatisticWindow::StatisticWindow(StatsProp::TypeSearch type, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StatisticWindow),
    _type(type)
{
    ui->setupUi(this);
    ui->comboBox_typeSearch->addItem("Прибыль за промежуток времени", StatsProp::Profit);//добавляем возможные поиски и статистики
    ui->comboBox_typeSearch->addItem("Популярность определенного типа одежды", StatsProp::TypeOfClothPopularity);
    ui->comboBox_typeSearch->addItem("Популярность всех видов одежды", StatsProp::AllTypeOfClothPopularity);
    ui->comboBox_typeSearch->addItem("Популярность использования материалов или фурнитуры в заказах", StatsProp::MaterialHardwarePopularity);

    ui->comboBox_lineType->addItem("Не сглаженный график", StatsProp::Line);
    ui->comboBox_lineType->addItem("Сглаженный график", StatsProp::Smooth);

    int currIndex = ui->comboBox_typeSearch->findData(_type);//задаем текущий индекс в комбобокс(принимаемый в конструкторе)
    ui->comboBox_typeSearch->setCurrentIndex(currIndex);

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    //ui->graphicsView->setRubberBand(QChartView::RectangleRubberBand);
    ui->dateEdit_from->setDate(QDate::currentDate());

    updateUI(_type, StatsProp::Line);
}

StatisticWindow::~StatisticWindow()
{
    delete ui;
}

StatsProp::TypeSearch StatisticWindow::type() const
{
    return _type;
}

void StatisticWindow::updateUI(StatsProp::TypeSearch type, StatsProp::LineType lineType)
{
    try {
        if(type == StatsProp::Profit)
        {
            ui->graphicsView->setRubberBand(QChartView::RectangleRubberBand);
            ui->comboBox_typeOfCloth->hide();

            QLineSeries *series;
            if(lineType == StatsProp::Line)
                series = new QLineSeries();
            else
                series = new QSplineSeries();
            QDate from(ui->dateEdit_from->date());
            QDate to(ui->dateEdit_to->date());

            QList<QDate> datesList = DBConnection::orderDates(from, to);
            QSet<QDate> datesSet = QSet<QDate>::fromList(datesList);//удаляет повторения в списке
            datesList = datesSet.values();//возвращаем список на место
            std::sort(datesList.begin(), datesList.end());
            QList<double> profitList = DBConnection::ordersProfit(datesList);
            for(int i(0); i < datesList.length(); i++)
            {
                QDateTime date(datesList[i]);
                series->append(date.toMSecsSinceEpoch(), profitList[i]);
            }
            series->setPointsVisible(true);

            QChart *chart = new QChart();
            chart->addSeries(series);
            chart->legend()->hide();
            chart->setTitle("Прибыль от " + from.toString("dd MMM yyyy") + " до " + to.toString("dd MMM yyyy"));
            //chart->setTheme(QChart::ChartThemeDark);
            chart->setAnimationOptions(QChart::AllAnimations);

            QDateTimeAxis *axisX = new QDateTimeAxis;
            axisX->setTickCount(10);
            axisX->setFormat("dd MMM yyyy");

            chart->addAxis(axisX, Qt::AlignBottom);
            series->attachAxis(axisX);

            QValueAxis *axisY = new QValueAxis;
            axisY->setLabelFormat("%i");
            axisY->setTitleText("Гривен");
            //axisY->setRange(0, *std::max_element(profitList.begin(), profitList.end()) + 1000);

            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisY);;
            ui->graphicsView->setChart(chart);
        }
        else if (type == StatsProp::TypeOfClothPopularity)
        {
            ui->graphicsView->setRubberBand(QChartView::RectangleRubberBand);
            if(ui->comboBox_typeOfCloth->isHidden())
            {
                ui->comboBox_typeOfCloth->show();
                ui->comboBox_typeOfCloth->clear();
                ui->comboBox_typeOfCloth->addItems(DBConnection::ToCNamesList());
            }
            else
            {
                QLineSeries *series;
                if(lineType == StatsProp::Line)
                    series = new QLineSeries();
                else
                    series = new QSplineSeries();

                series->setPointsVisible(true);

                QDate from(ui->dateEdit_from->date());
                QDate to(ui->dateEdit_to->date());
                QString name(ui->comboBox_typeOfCloth->currentText());

                QList<QDate> datesList = DBConnection::ToCDates(from, to, name);
                QSet<QDate> datesSet = QSet<QDate>::fromList(datesList);//удаляет повторения в списке
                datesList = datesSet.values();//возвращаем список на место

                std::sort(datesList.begin(), datesList.end());

                QList<int> countList = DBConnection::ToCCounts(datesList, name);
                qDebug() << countList << datesList;
                for(int i(0); i < datesList.length(); i++)
                {
                    QDateTime date(datesList[i]);
                    series->append(date.toMSecsSinceEpoch(), countList[i]);
                }

                QChart *chart = new QChart();
                chart->addSeries(series);//1

                chart->legend()->hide();
                chart->setTitle("Популярность " + name + " от " + from.toString("dd MMM yyyy") + " до " + to.toString("dd MMM yyyy"));
                //chart->setTheme(QChart::ChartThemeDark);
                chart->setAnimationOptions(QChart::AllAnimations);

                QDateTimeAxis *axisX = new QDateTimeAxis;
                axisX->setTickCount(10);
                axisX->setFormat("dd MMM yyyy");

                chart->addAxis(axisX, Qt::AlignBottom);//2
                series->attachAxis(axisX);//3

                QValueAxis *axisY = new QValueAxis;
                axisY->setLabelFormat("%i");
                axisY->setTitleText("Куплено раз");
                //axisY->setRange(0, *std::max_element(countList.begin(), countList.end()) + 2);

                chart->addAxis(axisY, Qt::AlignLeft);//2
                series->attachAxis(axisY);//3
                ui->graphicsView->setChart(chart);//4
            }//else(ui->comboBox_typeOfCloth->isHidden())
        }//else if (type == StatsProp::TypeOfClothPopularity)
        else if (type == StatsProp::AllTypeOfClothPopularity)
        {
            ui->graphicsView->setRubberBand(QChartView::NoRubberBand);
            if(!ui->comboBox_lineType->isHidden())
            {
                ui->comboBox_typeOfCloth->hide();
                ui->comboBox_lineType->hide();
            }
            else
            {
                QDate from(ui->dateEdit_from->date());
                QDate to(ui->dateEdit_to->date());

                int yearCount = to.year() - from.year();

                DonutBreakdownChart *donutBreakdown = new DonutBreakdownChart();
                donutBreakdown->setAnimationOptions(QChart::AllAnimations);
                donutBreakdown->setTitle("Популярность одежды с " + from.toString("yyyy MMM dd") + " до " + to.toString("yyyy MMM dd"));
                donutBreakdown->legend()->setAlignment(Qt::AlignRight);

                QStringList tocList = DBConnection::ToCNamesList();
                int colorsCounter(0);
                QSet<QString> tocListInLegend;
                for(int names(0); names < tocList.length(); names++)
                {
                    QPieSeries *series = new QPieSeries();
                    QDate startDate(from);
                    for(int years(0); years <= yearCount; years++)
                    {
                        QDate finishDate(startDate);
                        if(to.year() - finishDate.year() >= 1)
                            finishDate.setDate(finishDate.year() + 1, 1, 1);
                        else
                            finishDate = to;
                        int count = DBConnection::ToCCount(startDate, finishDate, tocList[names]);
                        if(count != 0)
                        {
                            series->append(tocList[names] + " " + startDate.toString("yyyy MMM dd") + " - " + finishDate.toString("yyyy MMM dd"), count);
                            tocListInLegend << tocList[names];
                        }
                        startDate = finishDate;
                    }//for years
                    series->setName(tocList[names]);
                    QColor color = Qt::red;
                    int cv = colorsCounter % 4;
                    if(cv == 0)
                        color = QColor("#9AD9CC");
                    else if(cv == 1)
                        color = QColor("#D9D684");
                    else if(cv == 2)
                        color = QColor("#AF93DA");
                    else if(cv == 3)
                        color = QColor("#D99279");
                    donutBreakdown->addBreakdownSeries(series, color);
                    colorsCounter++;
                }//for names
                ui->graphicsView->setChart(donutBreakdown);
            }
        }//else if (type == StatsProp::AllTypeOfClothPopularity)
    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void StatisticWindow::on_pushButton_update_clicked()
{
    StatsProp::LineType lineType = ui->comboBox_lineType->itemData(ui->comboBox_lineType->currentIndex()).value<StatsProp::LineType>();
    updateUI(_type, lineType);
}

void StatisticWindow::on_comboBox_typeSearch_activated(int index)
{
    _type = ui->comboBox_typeSearch->itemData(index).value<StatsProp::TypeSearch>();
    updateUI(_type);
}

void StatisticWindow::on_dateEdit_from_userDateChanged(const QDate &date)
{
    ui->dateEdit_to->setMinimumDate(date);
}

void StatisticWindow::on_comboBox_typeOfCloth_activated(const QString &/*arg1*/)
{
    StatsProp::LineType lineType = ui->comboBox_lineType->itemData(ui->comboBox_lineType->currentIndex()).value<StatsProp::LineType>();
    updateUI(_type, lineType);
}
