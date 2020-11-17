#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include "Data/settings.h"
#include "Data/log.h"
#include "Data/dbconnection.h"

#include "mainwindow.h"
#include "settingswindow.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void on_pushButton_settings_clicked();

    void on_pushButton_clicked();

private:
    Ui::LoginWindow *ui;
    MainWindow *mainWindow;
    SettingsWindow *settingsWindow;
};
#endif // LOGINWINDOW_H
