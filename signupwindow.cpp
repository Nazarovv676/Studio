#include "signupwindow.h"
#include "ui_signupwindow.h"

#include "Data/dbconnection.h"
#include "Data/log.h"

#include <QMessageBox>

signUpWindow::signUpWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::signUpWindow)
{
    ui->setupUi(this);
}

signUpWindow::~signUpWindow()
{
    delete ui;
}

void signUpWindow::on_lineEdit_pass_textChanged(const QString &arg1)
{
    if(arg1 != NULL)
        ui->lineEdit_rePass->setEnabled(true);
    else
        ui->lineEdit_rePass->setEnabled(false);
}

void signUpWindow::on_lineEdit_rePass_textChanged(const QString &arg1)
{
    if(arg1 == ui->lineEdit_pass->text())
        ui->pushButton_addUser->setEnabled(true);
    else
        ui->pushButton_addUser->setEnabled(false);
}

void signUpWindow::on_pushButton_addUser_clicked()
{
    try {
        DBConnection::addUser(ui->lineEdit_name->text(), ui->lineEdit_pass->text());
        QMessageBox::information(this, "Успех!", "Пользователь " + ui->lineEdit_name->text() + " успешно добавлен!");
        this->close();
    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }

}
