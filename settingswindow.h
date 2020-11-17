#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QMainWindow>

#include "Data/settings.h"
#include "Data/log.h"
#include "Data/dbconnection.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

    void showEvent(QShowEvent *) override;

    bool getShowUI() const;
    void setShowUI(bool value);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::SettingsWindow *ui;
    bool showUI;
};

#endif // SETTINGSWINDOW_H
