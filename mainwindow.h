#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dbconnection.h"
#include "log.h"
#include "settings.h"

#include "settingswindow.h"

#include <QMainWindow>
#include <QCloseEvent>
#include <QSqlTableModel>
#include <QRegularExpressionValidator>
#include <QIntValidator>
#include <QDoubleValidator>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //__________________________custom events or overrides_____________________
    void updateUI();

    void closeEvent(QCloseEvent *event) override;

    //__________________________genereted events(slots)____________________________
    void on_action_settings_triggered();

    void on_tabWidget_currentChanged(int index);

    void on_pushButton_addUser_clicked();

    void on_action_addCustomer_triggered();

    void on_action_addMatHar_triggered();

    void on_pushButton_addMaterial_clicked();

    void on_pushButton_addHardware_clicked();

    void on_pushButton_addToC_clicked();

    void on_action_addToC_triggered();

    void on_pushButton_addMaster_clicked();

    void on_action_addMaster_triggered();

    void on_pushButton_addOrderAddMaterial_clicked();

    void on_pushButton_addOrderDelMaterial_clicked();

    void on_pushButton_addOrderAddHard_clicked();

    void on_pushButton_addOrderDelHard_clicked();

    void on_pushButton_addOrder_clicked();

    void on_action_addOrder_triggered();

    void on_comboBox_searchTableName_activated(const QString &arg1);

    void on_lineEdit_searchId_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;

    SettingsWindow *settingsWindow;///Окно для настроек

    //_______________________________________Varibles__________________________________
    QStringList materialNameList, materialQuantList, hardNameList, hardQuantList;//Списки названий и цен для заказа

    QSqlTableModel *tableModel;

    QRegularExpression *rxEmail;
    QRegularExpression *rxString45_ru;
    QRegularExpression *rxStringNum45_eng_ru;
    QRegularExpression *rxNumTel;
};

#endif // MAINWINDOW_H
