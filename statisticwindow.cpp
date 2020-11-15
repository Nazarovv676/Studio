#include "statisticwindow.h"
#include "ui_statisticwindow.h"

#include "dbconnection.h"
#include "log.h"

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

    updateUI(_type, StatsProp::Line);

//    QBarSet *set0 = new QBarSet("Jane");
//    QBarSet *set1 = new QBarSet("John");
//    QBarSet *set2 = new QBarSet("Axel");
//    QBarSet *set3 = new QBarSet("Mary");
//    QBarSet *set4 = new QBarSet("Samantha");

//    *set0 << 1 << 2 << 3 << 4 << 5 << 6;
//    *set1 << 5 << 0 << 0 << 4 << 0 << 7;
//    *set2 << 3 << 5 << 8 << 13 << 8 << 5;
//    *set3 << 5 << 6 << 7 << 3 << 4 << 5;
//    *set4 << 9 << 7 << 5 << 3 << 1 << 2;
//    QBarSeries *series = new QBarSeries();
//    series->append(set0);
//    series->append(set1);
//    series->append(set2);
//    series->append(set3);
//    series->append(set4);
//    QChart *chart = new QChart();
//    chart->addSeries(series);
//    chart->setTitle("Simple barchart example");
//    chart->setAnimationOptions(QChart::SeriesAnimations);
//    QStringList categories;
//    categories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun";
//    QBarCategoryAxis *axisX = new QBarCategoryAxis();
//    axisX->append(categories);
//    chart->addAxis(axisX, Qt::AlignBottom);
//    series->attachAxis(axisX);

//    QValueAxis *axisY = new QValueAxis();
//    axisY->setRange(0,15);
//    chart->addAxis(axisY, Qt::AlignLeft);
//    series->attachAxis(axisY);
//    chart->legend()->setVisible(true);
//    chart->legend()->setAlignment(Qt::AlignBottom);
//    ui->graphicsView->setChart(chart);
//    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

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
        QLineSeries *series;
        if(lineType == StatsProp::Line)
            series = new QLineSeries();
        else
            series = new QSplineSeries();

        if(type == StatsProp::Profit)
        {
            QDate from(ui->dateEdit_from->date());
            QDate to(ui->dateEdit_to->date());

            QList<QDate> datesList = DBConnection::orderDates(from, to);
            QSet<QDate> datesListSet = QSet<QDate>::fromList(datesList);
            datesList = datesListSet.values();
            qSort(datesList.begin(), datesList.end());
            QList<double> profitList = DBConnection::ordersProfit(datesList);
            qDebug() << datesList << profitList;
            for(int i(0); i < datesList.length(); i++)
            {
                QDateTime date(datesList[i]);
                series->append(date.toMSecsSinceEpoch(), profitList[i]);
            }
            series->setPointsVisible(true);
            series->setColor(Qt::blue);

            QChart *chart = new QChart();
            chart->addSeries(series);
            chart->legend()->hide();
            chart->setTitle("Прибыль");
            chart->setTheme(QChart::ChartThemeDark);

            QDateTimeAxis *axisX = new QDateTimeAxis;
            axisX->setTickCount(10);
            axisX->setFormat("dd MMM yyyy");
            chart->addAxis(axisX, Qt::AlignBottom);
            series->attachAxis(axisX);

            QValueAxis *axisY = new QValueAxis;
            axisY->setLabelFormat("%i");
            axisY->setTitleText("Гривен");
            axisY->setRange(0, *std::max_element(profitList.begin(), profitList.end()) + 1000);

            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisY);
            chart->setAnimationOptions(QChart::AllAnimations);
            ui->graphicsView->setChart(chart);
        }
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
}
