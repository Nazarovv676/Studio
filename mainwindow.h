#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dbconnection.h"
#include "log.h"
#include "settings.h"

#include "settingswindow.h"
#include "signupwindow.h"

#include "orderwriter.h"

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

    void updateSearchTable();

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

    void on_lineEdit_search1_textChanged(const QString &arg1);

    void on_lineEdit_search2_textChanged(const QString &arg1);

    void on_lineEdit_search3_textChanged(const QString &arg1);

    void on_lineEdit_search4_textChanged(const QString &arg1);

    void on_lineEdit_search5_textChanged(const QString &arg1);

    void on_action_addUser_triggered();

    void on_pushButton_searchDelete_clicked();

    void on_tableView_search_clicked(const QModelIndex &index);

    void on_pushButton_searchDelMat_clicked();

    void on_pushButton_searchDelHar_clicked();

    void on_pushButton_searchAddMat_clicked();

    void on_pushButton_searchAddHar_clicked();

    void on_pushButton_searchExport_clicked();

private:
    void hideNonValidRowsSearchTable(int column, QString compareText);

    Ui::MainWindow *ui;
    SettingsWindow *settingsWindow;///Окно для настроек
    signUpWindow *signupWindow;
    QStringList materialNameList, materialQuantList, hardNameList, hardQuantList;//Списки названий и цен для заказа
    QSqlTableModel *tableModel;
    QSqlTableModel *tabModHar;
    QSqlTableModel *tabModMat;
    QRegularExpression *rxEmail;
    QRegularExpression *rxString45_ru;
    QRegularExpression *rxStringNum45_eng_ru;
    QRegularExpression *rxNumTel;
    QRegularExpression *emptiness;

    OrderWriter order;
};

#endif // MAINWINDOW_H
