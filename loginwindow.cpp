#include "loginwindow.h"
#include "ui_loginwindow.h"

#include <QMessageBox>
#include <QDebug>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);

    try {//открываем логи в отдельном try
        Log::open();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Ошибка", "Ошибка открытия файла логов");
        qApp->closeAllWindows();
    }

    try {
        Log::write("Program is open");
        setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);//так как размеры окна не меняются убираем кнопку развернуть и свернуть(свернуть просто так)
        //______________________________________
        QFont font("Nunito bold");//устанавливаем нужный размер шрифта
        font.setPointSize(Settings::settings.value("UI/FONTSIZE").toInt());
        qApp->setFont(font);

        Settings::updatePalette();//вызываем тут обновление палитры так как это первый доступный конструктор
        //_______________________________________________________
        if(Settings::settings.value("UI/THEME").toInt() == 0)//обновляем палитру
            qApp->setPalette(Settings::greyPalette);
        else if(Settings::settings.value("UI/THEME").toInt() == 1)
            qApp->setPalette(Settings::blackPalette);
        else
            qApp->setPalette(Settings::whitePalette);
        //___________________________________________________________

    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }

}

LoginWindow::~LoginWindow()
{
    delete settingsWindow;
    delete mainWindow;
    delete ui;
}

void LoginWindow::on_pushButton_clicked()
{
    try {
        DBConnection::userName = ui->lineEdit_userName->text();
        DBConnection::userPass = ui->lineEdit_userPass->text();
        DBConnection::connect();
        mainWindow = new MainWindow();//открываем главное окно если все подключилоось
        mainWindow->show();
        hide();
    } catch (const std::exception& e) {
        Log::write(e.what());
        Log::write(DBConnection::query.lastError().text());
        if(QString(e.what()).toInt() == 1045 || QString(e.what()).toInt() == 2059)
            QMessageBox::warning(this, "Ошибка", "Неправильный логин или пароль! Попробуйте еще раз.");
        else
            QMessageBox::critical(this, "Ошибка", "Ошибка связи с базой данных №" + QString(e.what()));
    }
}

void LoginWindow::on_pushButton_settings_clicked()
{
    settingsWindow = new SettingsWindow(this);
    settingsWindow->open();
}
