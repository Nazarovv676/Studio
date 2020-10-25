#include "settingswindow.h"
#include "ui_settingswindow.h"
#include <QDebug>

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::showEvent(QShowEvent *)
{
    ui->lineEdit_host->setText(Settings::settings.value("CONNECTION/HOST").toString());
    ui->lineEdit_port->setText(Settings::settings.value("CONNECTION/PORT").toString());
    ui->lineEdit_dbName->setText(Settings::settings.value("CONNECTION/DBNAME").toString());
    ui->comboBox_tabPos->setCurrentIndex(Settings::settings.value("UI/TABPOS").toInt());
    ui->spinBox_fontSize->setValue(Settings::settings.value("UI/FONTSIZE").toInt());
    ui->comboBox_theme->setCurrentIndex(Settings::settings.value("UI/THEME").toInt());

    //_______________________________
    if(!showUI)//если мы вручную не поставим разрешение то группа не покажется
        ui->groupBox_UI->hide();
    else
        ui->groupBox_UI->show();
}

void SettingsWindow::on_buttonBox_accepted()
{
    try {
        //запись в файл измененных данных
        Settings::settings.setValue("CONNECTION/HOST", ui->lineEdit_host->text());
        Settings::settings.setValue("CONNECTION/PORT", ui->lineEdit_port->text());
        Settings::settings.setValue("CONNECTION/DBNAME", ui->lineEdit_dbName->text());
        Settings::settings.setValue("UI/TABPOS", ui->comboBox_tabPos->currentIndex());
        Settings::settings.setValue("UI/FONTSIZE", ui->spinBox_fontSize->value());
        Settings::settings.setValue("UI/THEME", ui->comboBox_theme->currentIndex());

        //выгрузка из конфиг файла данных в переменные
        DBConnection::host = Settings::settings.value("CONNECTION/HOST").toString();
        DBConnection::dbName = Settings::settings.value("CONNECTION/DBNAME").toString();
        DBConnection::port = Settings::settings.value("CONNECTION/PORT").toString();

    } catch (const std::exception& e) {
        Log::write(e.what());
    }
}

bool SettingsWindow::getShowUI() const
{
    return showUI;
}

void SettingsWindow::setShowUI(bool value)
{
    showUI = value;
}
