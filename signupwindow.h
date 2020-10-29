#ifndef SignUpWindow_H
#define SignUpWindow_H

#include <QDialog>

namespace Ui {
class signUpWindow;
}

class signUpWindow : public QDialog
{
    Q_OBJECT

public:
    explicit signUpWindow(QWidget *parent = nullptr);
    ~signUpWindow();

private slots:
    void on_lineEdit_pass_textChanged(const QString &arg1);

    void on_lineEdit_rePass_textChanged(const QString &arg1);

    void on_pushButton_addUser_clicked();

private:
    Ui::signUpWindow *ui;
};

#endif // SignUpWindow_H
