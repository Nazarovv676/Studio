#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMessageBox>

#define MESSAGETIMEOUT 3000

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //__________________________________________________________________________________________________
    updateUI();//применяем настройки из файла к интерфейсу

    ui->label_search1->setText("Выберите имя таблицы еще раз для обновления информации");
    ui->label_search2->hide();
    ui->label_search3->hide();
    ui->label_search4->hide();
    ui->label_search5->hide();

    ui->lineEdit_search1->hide();
    ui->lineEdit_search2->hide();
    ui->lineEdit_search3->hide();
    ui->lineEdit_search4->hide();
    ui->lineEdit_search5->hide();
    //__________________________________________________________________________________________________
    for(int i(0); i < ui->tabWidget->count(); i++)//имитируем переход по всем вкладкам для обновления id при входе в программу
        on_tabWidget_currentChanged(i);


    tableModel = new QSqlTableModel(this, DBConnection::database);//задаем базу и выделяем память для модели(для выгрузки информации в таблицу)

    //_______________________ставим валидаторы в виде регулярных выражений на ввод________________________________________________________________________

    rxEmail = new QRegularExpression("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b", QRegularExpression::CaseInsensitiveOption);
    rxString45_ru = new QRegularExpression("[А-я -]{45}", QRegularExpression::CaseInsensitiveOption);
    rxStringNum45_eng_ru = new QRegularExpression("[А-яA-z0-9 -]{45}", QRegularExpression::CaseInsensitiveOption);
    rxNumTel = new QRegularExpression("\\+\\d{3}\\-\\d{2}\\-\\d{3}\\-\\d{2}\\-\\d{2}");

    ui->lineEdit_email->setValidator(new QRegularExpressionValidator(*rxEmail, this));

    ui->lineEdit_name->setValidator(new QRegularExpressionValidator(*rxString45_ru, this));
    ui->lineEdit_surname->setValidator(new QRegularExpressionValidator(*rxString45_ru, this));
    ui->lineEdit_addMasterName->setValidator(new QRegularExpressionValidator(*rxString45_ru, this));
    ui->lineEdit_addMasterSurname->setValidator(new QRegularExpressionValidator(*rxString45_ru, this));

    ui->addHarName->setValidator(new QRegularExpressionValidator(*rxStringNum45_eng_ru, this));
    ui->addMatName->setValidator(new QRegularExpressionValidator(*rxStringNum45_eng_ru, this));
    ui->lineEdit_addToCName->setValidator(new QRegularExpressionValidator(*rxStringNum45_eng_ru, this));
    ui->lineEdit_addMasterAddress->setValidator(new QRegularExpressionValidator(*rxStringNum45_eng_ru, this));

    ui->lineEdit_numTel->setValidator(new QRegularExpressionValidator(*rxNumTel, this));
    ui->lineEdit_addMasterNumTel->setValidator(new QRegularExpressionValidator(*rxNumTel, this));

}

MainWindow::~MainWindow()
{
    delete settingsWindow;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)//перегрузка события закрытия
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Выход", "Вы уверены, что хотите выйти из программы?", QMessageBox::No | QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes)
        event->ignore();
    else
    {
        Log::write("Program is close");//запись в логи о закритии програмы
        event->accept();
    }
}

void MainWindow::on_action_settings_triggered()
{
    settingsWindow = new SettingsWindow(this);
    settingsWindow->setShowUI(true);
    settingsWindow->open();
    connect(settingsWindow, SIGNAL(accepted()), this, SLOT(updateUI()), Qt::QueuedConnection);
}

void MainWindow::updateUI()//применяем настройки для интерфейса программы
{
    try {
        if(Settings::settings.value("UI/TABPOS").toInt() == 0)//настраиваем позицию вкладок
            ui->tabWidget->setTabPosition(QTabWidget::West);
        else
            ui->tabWidget->setTabPosition(QTabWidget::North);

        QFont font("Nunito bold");//обновляем шрифт и его размер
        font.setPointSize(Settings::settings.value("UI/FONTSIZE").toInt());
        qApp->setFont(font);

        if(Settings::settings.value("UI/THEME").toInt() == 0)//обновляем тему
            qApp->setPalette(Settings::greyPalette);
        else if(Settings::settings.value("UI/THEME").toInt() == 1)
            qApp->setPalette(Settings::blackPalette);
        else
            qApp->setPalette(Settings::whitePalette);

    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)//обновление id клиента во вкладках при листании вкладок
{
    try {
        if(ui->tabWidget->tabText(index) == "Регистрация клиента")
            ui->lineEdit_id->setText(QString::number(DBConnection::customersCount() + 1));
        else if(ui->tabWidget->tabText(index) == "Регистрация мастера")
            ui->lineEdit_addMasterID->setText(QString::number(DBConnection::masterCount() + 1));
        //        else if(ui->tabWidget->tabText(index) == "Поиск")
        //            on_comboBox_searchTableName_activated(ui->comboBox_searchTableName->currentText());
        else if(ui->tabWidget->tabText(index) == "Оформление заказа")
        {
            ui->lineEdit_addOrderIdOrder->setText(QString::number(DBConnection::orderCount() + 1));

            ui->comboBox_addOrderToCName->clear();//очситка комбобоксов для предотвращения дубликатов
            ui->comboBox_addOrderHardName->clear();
            ui->comboBox_addOrderMaterialName->clear();

            QSqlQuery q = DBConnection::ToCList();//Обновление списков в комбобоксах
            while(q.next())
                ui->comboBox_addOrderToCName->addItem(q.value(0).toString());
            q = DBConnection::materialList();
            while(q.next())
                ui->comboBox_addOrderMaterialName->addItem(q.value(0).toString());
            q = DBConnection::hardwareList();
            while(q.next())
                ui->comboBox_addOrderHardName->addItem(q.value(0).toString());
        }
    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void MainWindow::on_pushButton_addUser_clicked()//добавление клиента в базу
{
    try {
        if(ui->lineEdit_name->text() == NULL)
            QMessageBox::warning(this, "Ошибка!", "Введите имя клиента!!!");
        else if(ui->lineEdit_surname->text() == NULL)
            QMessageBox::warning(this, "Ошибка!", "Введите фамилию клиента!!!");
        else if(ui->lineEdit_numTel->text() == NULL)
            QMessageBox::warning(this, "Ошибка!", "Введите номер телефона клиента!!!");
        else if(ui->lineEdit_email->text() == NULL)
            QMessageBox::warning(this, "Ошибка!", "Введите адрес электронной почты клиента!!!");
        else if(DBConnection::containsClient(ui->lineEdit_numTel->text()))
            QMessageBox::warning(this, "Ошибка!", "Клиент с таким номером уже зарегистрирован!!!");
        else if(!ui->lineEdit_numTel->hasAcceptableInput())
            QMessageBox::warning(this, "Ошибка!", "Номер телефона не поддерживаемого формата!!!");
        else if(!ui->lineEdit_email->hasAcceptableInput())
            QMessageBox::warning(this, "Ошибка!", "Введите валидный адрес электронной почты!!!");//проверили готовность данных для ввода в базу
        else
        {
            DBConnection::addCustomer(ui->lineEdit_name->text(), ui->lineEdit_surname->text(), ui->lineEdit_numTel->text(), ui->lineEdit_email->text());
            QMessageBox::information(this, "Успешно!", "Клиент " + ui->lineEdit_surname->text() + " " + ui->lineEdit_name->text() + " теперь есть в базе! ID: " + DBConnection::getCustIdByNumTel(ui->lineEdit_numTel->text()));//добавить метод для поиска по телеону(телефоны в базе не повторяются)
        }
    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }

}

//___________________________________________________________________________________________________________________________

void MainWindow::on_action_addCustomer_triggered()//открытие вкладки добавления клиента
{
    for(int i(0); i < ui->tabWidget->count(); i++)
        if(ui->tabWidget->tabText(i) == "Регистрация клиента")
            ui->tabWidget->setCurrentIndex(i);
}

void MainWindow::on_action_addMatHar_triggered()//открытие вкладки поставки
{
    for(int i(0); i < ui->tabWidget->count(); i++)
        if(ui->tabWidget->tabText(i) == "Поставка")
            ui->tabWidget->setCurrentIndex(i);
}

void MainWindow::on_action_addToC_triggered()//открытие вкладки добавления типа одежды
{
    for(int i(0); i < ui->tabWidget->count(); i++)
        if(ui->tabWidget->tabText(i) == "Добавить вариант одежды")
            ui->tabWidget->setCurrentIndex(i);
}

void MainWindow::on_action_addMaster_triggered()//открытие вкладки добавления мастера
{
    for(int i(0); i < ui->tabWidget->count(); i++)
        if(ui->tabWidget->tabText(i) == "Регистрация мастера")
            ui->tabWidget->setCurrentIndex(i);
}

void MainWindow::on_action_addOrder_triggered()//открытие вкладки оформления заказа
{
    for(int i(0); i < ui->tabWidget->count(); i++)
        if(ui->tabWidget->tabText(i) == "Оформление заказа")
            ui->tabWidget->setCurrentIndex(i);
}
//__________________________________________________________________________________________

void MainWindow::on_pushButton_addMaterial_clicked()//добавление в базу материала
{
    try {
        if(ui->addMatName->text() == NULL)
            QMessageBox::warning(this, "Ошибка!", "Введите наименование материала!!!");
        else
            DBConnection::addMaterial(ui->addMatName->text(), ui->doubleSpinBox_matPrice->text(), ui->spinBox_matQuan->text());
        ui->statusbar->showMessage("Информация про материал успешно добавлена/обновлена!!!", MESSAGETIMEOUT);
    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void MainWindow::on_pushButton_addHardware_clicked()//добавление в базу фурнитуры
{
    try {
        if(ui->addHarName->text() == NULL)
            QMessageBox::warning(this, "Ошибка!", "Введите наименование фурнитуры!!!");
        else
            DBConnection::addHardware(ui->addHarName->text(), ui->doubleSpinBox_harPrice->text(), ui->spinBox_harQuan->text());
        ui->statusbar->showMessage("Информация про фурнитуру успешно добавлена/обновлена!!!", MESSAGETIMEOUT);
    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void MainWindow::on_pushButton_addToC_clicked()//добавление в базу типа одежды
{
    try {
        if(ui->lineEdit_addToCName->text() == NULL)
            QMessageBox::warning(this, "Ошибка!", "Введите наименование типа одежды!!!");
        else if(DBConnection::containsTypeOfCloth(ui->lineEdit_addToCName->text()))
        {
            QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Ателье", "Тип одежды с таким наименованием уже существует! Обновить данные полей \"Цена\" и \"Кол-во рабочих дней на изготовление\"?\n", QMessageBox::Yes | QMessageBox::Cancel);
            if(resBtn == QMessageBox::Yes)
            {
                DBConnection::addTypeOfCloth(ui->lineEdit_addToCName->text(), ui->doubleSpinBox_addToCPrice->text(), ui->spinBox_addToCDays->text());
                ui->statusbar->showMessage("Запись \"" + ui->lineEdit_addToCName->text() + "\" успешно добавлено", MESSAGETIMEOUT);
            }
        }
        else
        {
            DBConnection::addTypeOfCloth(ui->lineEdit_addToCName->text(), ui->doubleSpinBox_addToCPrice->text(), ui->spinBox_addToCDays->text());
            ui->statusbar->showMessage("Запись \"" + ui->lineEdit_addToCName->text() + "\" успешно добавлено", MESSAGETIMEOUT);
        }
    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void MainWindow::on_pushButton_addMaster_clicked()//добавление в базу мастера
{
    try {
        if(ui->lineEdit_addMasterName->text() == NULL)
            QMessageBox::warning(this, "Ошибка!", "Введите имя!!!");
        else if(ui->lineEdit_addMasterSurname->text() == NULL)
            QMessageBox::warning(this, "Ошибка!", "Введите фамилию!!!");
        else if(ui->lineEdit_addMasterNumTel->text() == NULL)
            QMessageBox::warning(this, "Ошибка!", "Введите номер телефона!!!");
        else if(ui->lineEdit_addMasterAddress->text() == NULL)
            QMessageBox::warning(this, "Ошибка!", "Введите адрес!!!");
        else if(DBConnection::containsMaster(ui->lineEdit_addMasterNumTel->text()))
            QMessageBox::warning(this, "Ошибка!", "Мастер с таким номером уже зарегистрирован!!!");
        else if(!ui->lineEdit_addMasterNumTel->hasAcceptableInput())
            QMessageBox::warning(this, "Ошибка!", "Номер телефона не поддерживаемого формата!!!");
        //проверили готовность данных для ввода в базу
        else
        {
            DBConnection::addMaster(ui->lineEdit_addMasterName->text(), ui->lineEdit_addMasterSurname->text(), ui->lineEdit_addMasterAddress->text(), ui->lineEdit_addMasterNumTel->text());
            QMessageBox::information(this, "Успешно!", "Мастер " + ui->lineEdit_addMasterSurname->text() + " " + ui->lineEdit_addMasterName->text() + " теперь есть в базе! ID: " + DBConnection::getMasterIdByNumTel(ui->lineEdit_addMasterNumTel->text()));//добавить метод для поиска по телеону(телефоны в базе не повторяются)
        }
    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void MainWindow::on_pushButton_addOrderAddMaterial_clicked()//добавление в листбокс
{
    materialNameList << ui->comboBox_addOrderMaterialName->currentText();
    materialQuantList << ui->doubleSpinBox_addOrderMaterialQuant->text().replace(",", ".");

    ui->listWidget_addOrderMaterialList->addItem(ui->comboBox_addOrderMaterialName->currentText() + '\t' + ui->doubleSpinBox_addOrderMaterialQuant->text().replace(",", "."));
}

void MainWindow::on_pushButton_addOrderDelMaterial_clicked()
{
    QList<QListWidgetItem*> items = ui->listWidget_addOrderMaterialList->selectedItems();
    foreach(QListWidgetItem * item, items)
    {
        materialNameList.removeAt(ui->listWidget_addOrderMaterialList->row(item));
        materialQuantList.removeAt(ui->listWidget_addOrderMaterialList->row(item));
        delete ui->listWidget_addOrderMaterialList->takeItem(ui->listWidget_addOrderMaterialList->row(item));
    }
}

void MainWindow::on_pushButton_addOrderAddHard_clicked()
{
    hardNameList << ui->comboBox_addOrderHardName->currentText();
    hardQuantList << ui->spinBox_addOrderHardQuant->text().replace(",", ".");

    ui->listWidget_addOrderHardList->addItem(ui->comboBox_addOrderHardName->currentText() + '\t' + ui->spinBox_addOrderHardQuant->text().replace(",", "."));
}

void MainWindow::on_pushButton_addOrderDelHard_clicked()
{
    QList<QListWidgetItem*> items = ui->listWidget_addOrderHardList->selectedItems();
    foreach(QListWidgetItem * item, items)
    {
        hardNameList.removeAt(ui->listWidget_addOrderHardList->row(item));
        hardQuantList.removeAt(ui->listWidget_addOrderHardList->row(item));
        delete ui->listWidget_addOrderHardList->takeItem(ui->listWidget_addOrderHardList->row(item));
    }
}

void MainWindow::on_pushButton_addOrder_clicked()
{
    try {
        if(!DBConnection::containsClient(ui->spinBox_addOrderCustId->value()))
            QMessageBox::warning(this, "Ошибка!", "Нет клиента с таким ID в базе!!!");
        else if(!DBConnection::containsMaster(ui->spinBox_addOrderMasterId->value()))
            QMessageBox::warning(this, "Ошибка!", "Нет мастера с таким ID в базе!!!");
        else if(ui->listWidget_addOrderMaterialList->count() == 0)
            QMessageBox::warning(this, "Ошибка!", "Список материалов пуст!!!");
        else
        {
            DBConnection::addOrder(ui->spinBox_addOrderCustId->text(), ui->spinBox_addOrderMasterId->text(), ui->comboBox_addOrderToCName->currentText(), materialNameList, materialQuantList, hardNameList, hardQuantList);

            materialNameList.clear();//очистка списков материалов и фурнитуры и их цен
            materialQuantList.clear();
            hardNameList.clear();
            hardQuantList.clear();

            QMessageBox::information(this, "Успех!", "Заказ добавлен в базу!");
        }
    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void MainWindow::on_comboBox_searchTableName_activated(const QString &tableName)
{
    try {

        if(tableName == "Заказ")
        {
            tableModel->setTable("order");
            tableModel->setHeaderData(0, Qt::Horizontal, "ID", Qt::DisplayRole);
            tableModel->setHeaderData(1, Qt::Horizontal, "Дата", Qt::DisplayRole);
            tableModel->setHeaderData(2, Qt::Horizontal, "ID мастера", Qt::DisplayRole);
            tableModel->setHeaderData(3, Qt::Horizontal, "ID клиента", Qt::DisplayRole);
            tableModel->setHeaderData(4, Qt::Horizontal, "Тип одежды", Qt::DisplayRole);

            //показываем все надписи
            ui->label_search1->show();
            ui->label_search2->show();
            ui->label_search3->show();
            ui->label_search4->show();
            ui->label_search5->show();

            //показываем все строки для ввода
            ui->lineEdit_search1->show();
            ui->lineEdit_search2->show();
            ui->lineEdit_search3->show();
            ui->lineEdit_search4->show();
            ui->lineEdit_search5->show();

            //установка надписей на фильтровых поисках
            ui->label_search1->setText("ID:");
            ui->label_search2->setText("Дата:");
            ui->label_search3->setText("ID мастера:");
            ui->label_search4->setText("ID клиента:");
            ui->label_search5->setText("Тип одежды:");
        }
        else if(tableName == "Клиент")
        {
            tableModel->setTable("customer");
            tableModel->setHeaderData(0, Qt::Horizontal, "ID", Qt::DisplayRole);
            tableModel->setHeaderData(1, Qt::Horizontal, "Имя", Qt::DisplayRole);
            tableModel->setHeaderData(2, Qt::Horizontal, "Фамилия", Qt::DisplayRole);
            tableModel->setHeaderData(3, Qt::Horizontal, "Номер телефона", Qt::DisplayRole);
            tableModel->setHeaderData(4, Qt::Horizontal, "Электронная почта", Qt::DisplayRole);

            //показываем все надписи
            ui->label_search1->show();
            ui->label_search2->show();
            ui->label_search3->show();
            ui->label_search4->show();
            ui->label_search5->show();

            //показываем все строки для ввода
            ui->lineEdit_search1->show();
            ui->lineEdit_search2->show();
            ui->lineEdit_search3->show();
            ui->lineEdit_search4->show();
            ui->lineEdit_search5->show();

            //установка надписей на фильтровых поисках
            ui->label_search1->setText("ID:");
            ui->label_search2->setText("Имя:");
            ui->label_search3->setText("Фамилия:");
            ui->label_search4->setText("Номер телефона:");
            ui->label_search5->setText("Электронная почта:");
        }
        else if(tableName == "Мастер")
        {
            tableModel->setTable("master");
            tableModel->setHeaderData(0, Qt::Horizontal, "ID", Qt::DisplayRole);
            tableModel->setHeaderData(1, Qt::Horizontal, "Имя", Qt::DisplayRole);
            tableModel->setHeaderData(2, Qt::Horizontal, "Фамилия", Qt::DisplayRole);
            tableModel->setHeaderData(3, Qt::Horizontal, "Адрес", Qt::DisplayRole);
            tableModel->setHeaderData(4, Qt::Horizontal, "Номер телефона", Qt::DisplayRole);

            //показываем все надписи
            ui->label_search1->show();
            ui->label_search2->show();
            ui->label_search3->show();
            ui->label_search4->show();
            ui->label_search5->show();

            //показываем все строки для ввода
            ui->lineEdit_search1->show();
            ui->lineEdit_search2->show();
            ui->lineEdit_search3->show();
            ui->lineEdit_search4->show();
            ui->lineEdit_search5->show();

            //установка надписей на фильтровых поисках
            ui->label_search1->setText("ID:");
            ui->label_search2->setText("Имя:");
            ui->label_search3->setText("Фамилия:");
            ui->label_search4->setText("Адрес:");
            ui->label_search5->setText("Номер телефона:");
        }
        else if(tableName == "Тип одежды")
        {
            tableModel->setTable("typeofcloth");
            tableModel->setHeaderData(0, Qt::Horizontal, "Наименование", Qt::DisplayRole);
            tableModel->setHeaderData(1, Qt::Horizontal, "Рабочих дней на изготовление", Qt::DisplayRole);
            tableModel->setHeaderData(2, Qt::Horizontal, "Цена", Qt::DisplayRole);

            //показываем нужные надписи
            ui->label_search1->show();
            ui->label_search2->show();
            ui->label_search3->show();
            ui->label_search4->hide();
            ui->label_search5->hide();

            //показываем нужные строки для ввода
            ui->lineEdit_search1->show();
            ui->lineEdit_search2->show();
            ui->lineEdit_search3->show();
            ui->lineEdit_search4->hide();
            ui->lineEdit_search5->hide();

            //установка надписей на фильтровых поисках
            ui->label_search1->setText("Наименование:");
            ui->label_search2->setText("Рабочих дней на изготовление:");
            ui->label_search3->setText("Цена:");
        }
        else if(tableName == "Фурнитура")
        {
            tableModel->setTable("hardware");
            tableModel->setHeaderData(0, Qt::Horizontal, "Наименование", Qt::DisplayRole);
            tableModel->setHeaderData(1, Qt::Horizontal, "Цена", Qt::DisplayRole);
            tableModel->setHeaderData(2, Qt::Horizontal, "Количество в наличии", Qt::DisplayRole);

            //показываем нужные надписи
            ui->label_search1->show();
            ui->label_search2->show();
            ui->label_search3->show();
            ui->label_search4->hide();
            ui->label_search5->hide();

            //показываем нужные строки для ввода
            ui->lineEdit_search1->show();
            ui->lineEdit_search2->show();
            ui->lineEdit_search3->show();
            ui->lineEdit_search4->hide();
            ui->lineEdit_search5->hide();

            //установка надписей на фильтровых поисках
            ui->label_search1->setText("Наименование:");
            ui->label_search2->setText("Цена:");
            ui->label_search3->setText("Количество в наличии:");
        }
        else if(tableName == "Материал")
        {
            tableModel->setTable("material");
            tableModel->setHeaderData(0, Qt::Horizontal, "Наименование", Qt::DisplayRole);
            tableModel->setHeaderData(1, Qt::Horizontal, "Цена", Qt::DisplayRole);
            tableModel->setHeaderData(2, Qt::Horizontal, "Количество в наличии", Qt::DisplayRole);

            //показываем нужные надписи
            ui->label_search1->show();
            ui->label_search2->show();
            ui->label_search3->show();
            ui->label_search4->hide();
            ui->label_search5->hide();

            //показываем нужные строки для ввода
            ui->lineEdit_search1->show();
            ui->lineEdit_search2->show();
            ui->lineEdit_search3->show();
            ui->lineEdit_search4->hide();
            ui->lineEdit_search5->hide();

            //установка надписей на фильтровых поисках
            ui->label_search1->setText("Наименование:");
            ui->label_search2->setText("Цена:");
            ui->label_search3->setText("Количество в наличии:");
        }
        else if(tableName == "Списки материалов в заказах")
        {
            tableModel->setTable("material_order");
            tableModel->setHeaderData(0, Qt::Horizontal, "Количество", Qt::DisplayRole);
            tableModel->setHeaderData(1, Qt::Horizontal, "Наименование материала", Qt::DisplayRole);
            tableModel->setHeaderData(2, Qt::Horizontal, "ID заказа", Qt::DisplayRole);

            //показываем нужные надписи
            ui->label_search1->show();
            ui->label_search2->show();
            ui->label_search3->show();
            ui->label_search4->hide();
            ui->label_search5->hide();

            //показываем нужные строки для ввода
            ui->lineEdit_search1->show();
            ui->lineEdit_search2->show();
            ui->lineEdit_search3->show();
            ui->lineEdit_search4->hide();
            ui->lineEdit_search5->hide();

            //установка надписей на фильтровых поисках
            ui->label_search1->setText("Количество:");
            ui->label_search2->setText("Наименование материала:");
            ui->label_search3->setText("ID заказа:");
        }
        else if(tableName == "Списки фурнитуры в заказах")
        {
            tableModel->setTable("hardware_order");
            tableModel->setHeaderData(0, Qt::Horizontal, "Количество", Qt::DisplayRole);
            tableModel->setHeaderData(1, Qt::Horizontal, "Наименование Фурнитуры", Qt::DisplayRole);
            tableModel->setHeaderData(2, Qt::Horizontal, "ID заказа", Qt::DisplayRole);

            //показываем нужные надписи
            ui->label_search1->show();
            ui->label_search2->show();
            ui->label_search3->show();
            ui->label_search4->hide();
            ui->label_search5->hide();

            //показываем нужные строки для ввода
            ui->lineEdit_search1->show();
            ui->lineEdit_search2->show();
            ui->lineEdit_search3->show();
            ui->lineEdit_search4->hide();
            ui->lineEdit_search5->hide();

            //установка надписей на фильтровых поисках
            ui->label_search1->setText("Количество:");
            ui->label_search2->setText("Наименование фурнитуры:");
            ui->label_search3->setText("ID заказа:");
        }

        tableModel->select();//взятие данных из таблиц
        ui->tableView_search->setModel(tableModel);
    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void MainWindow::hideNonValidRowsSearchTable(int column, QString compareText)
{
    for(int i(0); i < tableModel->rowCount(); i++)
        if(compareText.contains("<"))
        {
            QString compareTextToNum = compareText;
            compareTextToNum.remove("<");
            if (tableModel->data(tableModel->index(i, column)).toDouble() > compareTextToNum.toDouble())
                ui->tableView_search->hideRow(i);
        }
        else if(compareText.contains(">"))
        {
            QString compareTextToNum = compareText;
            compareTextToNum.remove(">");
            if (tableModel->data(tableModel->index(i, column)).toDouble() < compareTextToNum.toDouble())
                ui->tableView_search->hideRow(i);
        }
        else
        {
            if(!tableModel->data(tableModel->index(i, column)).toString().contains(compareText))
            ui->tableView_search->hideRow(i);
        }
}

void MainWindow::updateSearchTable()
{
    try {
        for(int i(0); i < tableModel->rowCount(); i++)
            ui->tableView_search->showRow(i);


        if(!ui->lineEdit_search1->isHidden())
            hideNonValidRowsSearchTable(0, ui->lineEdit_search1->text());

        if(!ui->lineEdit_search2->isHidden())
            hideNonValidRowsSearchTable(1, ui->lineEdit_search2->text());

        if(!ui->lineEdit_search3->isHidden())
            hideNonValidRowsSearchTable(2, ui->lineEdit_search3->text());

        if(!ui->lineEdit_search4->isHidden())
            hideNonValidRowsSearchTable(3, ui->lineEdit_search4->text());

        if(!ui->lineEdit_search5->isHidden())
            hideNonValidRowsSearchTable(4, ui->lineEdit_search5->text());

    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }

}

void MainWindow::on_lineEdit_search1_textChanged(const QString &arg1)
{
    qDebug() << arg1;
    updateSearchTable();
}

void MainWindow::on_lineEdit_search2_textChanged(const QString &arg1)
{
    qDebug() << arg1;
    updateSearchTable();
}

void MainWindow::on_lineEdit_search3_textChanged(const QString &arg1)
{
    qDebug() << arg1;
    updateSearchTable();
}

void MainWindow::on_lineEdit_search4_textChanged(const QString &arg1)
{
    qDebug() << arg1;
    updateSearchTable();
}

void MainWindow::on_lineEdit_search5_textChanged(const QString &arg1)
{
    qDebug() << arg1;
    updateSearchTable();
}
