#ifndef STATISTICWINDOW_H
#define STATISTICWINDOW_H

#include <QMainWindow>

namespace StatsProp {
enum TypeSearch{
    Profit = 1,
    AllTypeOfClothPopularity = 2,
    TypeOfClothPopularity = 3,
    MaterialHardwarePopularity = 4
};
enum LineType{
    Smooth,
    Line
};
}

Q_DECLARE_METATYPE(StatsProp::TypeSearch)
Q_DECLARE_METATYPE(StatsProp::LineType)

namespace Ui {
class StatisticWindow;
}

class StatisticWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit StatisticWindow(StatsProp::TypeSearch type = StatsProp::Profit, QWidget *parent = nullptr);
    ~StatisticWindow();

    StatsProp::TypeSearch type() const;

public slots:
    void updateUI(StatsProp::TypeSearch type, StatsProp::LineType lineType);

private slots:
    void on_pushButton_update_clicked();

    void on_comboBox_typeSearch_activated(int index);

private:
    Ui::StatisticWindow *ui;

    StatsProp::TypeSearch _type;
};

#endif // STATISTICWINDOW_H
