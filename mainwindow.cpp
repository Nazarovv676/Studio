#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ItemDelegate/itemdelegate.h"
#include "ItemDelegate/itemdelegatecombobox.h"
#include "ItemDelegate/itemdelegatecheckbox.h"
#include "ItemDelegate/itemdelegatespinbox.h"
#include "ItemDelegate/itemdelegatedoublespinbox.h"
#include "ItemDelegate/itemdelegatedate.h"

#include <QDebug>
#include <QMessageBox>

constexpr auto MESSAGETIMEOUT = 3000;
constexpr auto MINSPINBOXVALUE = -999999999;
constexpr auto MAXSPINBOXVALUE = 999999999;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(windowTitle() + " - " + DBConnection::userName);
    tableModel = new QSqlTableModel(this, DBConnection::database);//задаем базу и выделяем память для модели(для выгрузки информации в таблицу)
    tableModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    updateUI();//применяем настройки из файла к интерфейсу
    //убираем пока не нужные виждеты в поиске
    ui->label_search1->setText("Выберите имя таблицы еще раз для обновления информации");
    ui->label_search2->hide();
    ui->label_search3->hide();
    ui->label_search4->hide();
    ui->label_search5->hide();
    ui->radioButton_searchCompleted->hide();
    ui->radioButton_searchUnfulfilled->hide();
    ui->lineEdit_search1->hide();
    ui->lineEdit_search2->hide();
    ui->lineEdit_search3->hide();
    ui->lineEdit_search4->hide();
    ui->lineEdit_search5->hide();
    ui->tableView_searchHardware->hide();//спрячем таблички для вывода информации в информационнные таблички поиска
    ui->tableView_searchMaterial->hide();
    ui->pushButton_searchExport->hide();
    ui->splitter_searchTables->setSizes(QList<int>() << 1000 << 0);
    ui->splitter_searchTables->setCollapsible(0, false);
    ui->splitter_searchTables->handle(1)->setEnabled(false);
    //убираем доступ к регистрации пользователя если не root
    if(DBConnection::userName != "root")
    {
        ui->action_addUser->setEnabled(false);
        ui->action_addUser->setToolTip("Для доступа к привилегиям зайдите с помощью имени 'root'");
    }

    for(int i(0); i < ui->tabWidget->count(); i++)//имитируем переход по всем вкладкам для обновления id при входе в программу
        on_tabWidget_currentChanged(i);

    //_______________________ставим валидаторы в виде регулярных выражений на ввод________________________________________________________________________

    rxEmail = new QRegularExpression("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b", QRegularExpression::CaseInsensitiveOption);
    rxString45_ru = new QRegularExpression("[А-я -]{1,45}", QRegularExpression::CaseInsensitiveOption);
    rxStringNum45_eng_ru = new QRegularExpression("[А-яA-z0-9 -]{1,45}", QRegularExpression::CaseInsensitiveOption);
    rxNumTel = new QRegularExpression("\\+\\d{3}\\-\\d{2}\\-\\d{3}\\-\\d{2}\\-\\d{2}");
    emptiness = new QRegularExpression("{0}", QRegularExpression::CaseInsensitiveOption);

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
    ui->lineEdit_addOrderCustTel->setValidator(new QRegularExpressionValidator(*rxNumTel, this));
}

MainWindow::~MainWindow()
{
    delete ui;
    settingsWindow->close();
    delete settingsWindow;
    signupWindow->close();
    delete signupWindow;
    statsWindow->close();
    delete statsWindow;
    delete tableModel;
    delete tabModHar;
    delete tabModMat;
    delete rxEmail;
    delete rxString45_ru;
    delete rxStringNum45_eng_ru;
    delete rxNumTel;
    delete emptiness;
}

void MainWindow::closeEvent(QCloseEvent *event)//перегрузка события закрытия
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Выход", "Вы уверены, что хотите выйти из программы?", QMessageBox::No | QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes)
        event->ignore();
    else
    {
        Log::write("Program is close");//запись в логи о закритии програмы
        DBConnection::database.close();
        event->accept();
    }
}

void MainWindow::on_action_settings_triggered()//открытие окна настроек
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
        if(ui->tabWidget->tabText(index) == "Оформление заказа")
        {
            ui->comboBox_addOrderToCName->clear();//очситка комбобоксов для предотвращения дубликатов
            ui->comboBox_addOrderHardName->clear();
            ui->comboBox_addOrderMaterialName->clear();
            ui->comboBox_addOrderMaster->clear();

            QSqlQuery q = DBConnection::qToCList();//Обновление списков в комбобоксах
            while(q.next())
                ui->comboBox_addOrderToCName->addItem(q.value(0).toString());
            q = DBConnection::qMaterialNameList();
            while(q.next())
                ui->comboBox_addOrderMaterialName->addItem(q.value(0).toString());
            q = DBConnection::qHardwareNameList();
            while(q.next())
                ui->comboBox_addOrderHardName->addItem(q.value(0).toString());
            q = DBConnection::qMasterList();
            while(q.next())
                ui->comboBox_addOrderMaster->addItem(q.value(0).toString() + " " + q.value(1).toString() + " " + q.value(2).toString() + " " + q.value(4).toString());
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
        else if(DBConnection::containsCust(ui->lineEdit_numTel->text()))
            QMessageBox::warning(this, "Ошибка!", "Клиент с таким номером уже зарегистрирован!!!");
        else if(!ui->lineEdit_numTel->hasAcceptableInput())
            QMessageBox::warning(this, "Ошибка!", "Номер телефона не поддерживаемого формата!!!");
        else if(!ui->lineEdit_email->hasAcceptableInput())
            QMessageBox::warning(this, "Ошибка!", "Введите валидный адрес электронной почты!!!");//проверили готовность данных для ввода в базу
        else
        {
            DBConnection::addCustomer(ui->lineEdit_name->text(), ui->lineEdit_surname->text(), ui->lineEdit_numTel->text(), ui->lineEdit_email->text());
            QMessageBox::information(this, "Успешно!", "Клиент " + ui->lineEdit_surname->text() + " " + ui->lineEdit_name->text() + " теперь есть в базе! Номер: " + DBConnection::custIdByNumTel(ui->lineEdit_numTel->text()));//добавить метод для поиска по телеону(телефоны в базе не повторяются)
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
            DBConnection::addMaterial(ui->addMatName->text(), ui->doubleSpinBox_matPrice->text(), ui->doubleSpinBox_matQuan->text());
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
            DBConnection::addHardware(ui->addHarName->text(), ui->doubleSpinBox_harPrice->text(), ui->doubleSpinBox_harQuan->text());
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
            QMessageBox::information(this, "Успешно!", "Мастер " + ui->lineEdit_addMasterSurname->text() + " " + ui->lineEdit_addMasterName->text() + " теперь есть в базе! Номер: " + DBConnection::masterIdByNumTel(ui->lineEdit_addMasterNumTel->text()));//добавить метод для поиска по телеону(телефоны в базе не повторяются)
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
    hardQuantList << ui->doubleSpinBox_addOrderHardQuant->text().replace(",", ".");

    ui->listWidget_addOrderHardList->addItem(ui->comboBox_addOrderHardName->currentText() + '\t' + ui->doubleSpinBox_addOrderHardQuant->text().replace(",", "."));
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
        if(!DBConnection::containsCust(ui->lineEdit_addOrderCustTel->text()))
            QMessageBox::warning(this, "Ошибка!", "Нет клиента с таким номером телефона в базе!!!");
        //        else if(!DBConnection::containsMaster(ui->lineEdit_addOrderMasterTel->text()))
        //            QMessageBox::warning(this, "Ошибка!", "Нет мастера с таким номером телефона в базе!!!");
        else if(ui->listWidget_addOrderMaterialList->count() == 0)
            QMessageBox::warning(this, "Ошибка!", "Список материалов пуст!!!");
        else
        {
            QString orderID = DBConnection::addOrder(DBConnection::custID(ui->lineEdit_addOrderCustTel->text()), ui->comboBox_addOrderMaster->currentText().section(' ', 0, 0), ui->comboBox_addOrderToCName->currentText(), materialNameList, materialQuantList, hardNameList, hardQuantList);

            materialNameList.clear();//очистка списков материалов и фурнитуры и их цен
            materialQuantList.clear();
            hardNameList.clear();
            hardQuantList.clear();

            QMessageBox::information(this, "Успех!", "Заказ добавлен в базу! Номер: " + orderID);
            ui->listWidget_addOrderHardList->clear();
            ui->listWidget_addOrderMaterialList->clear();
            order.write(orderID);
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
            ui->tableView_search->setSortingEnabled(false);

            tableModel->setTable("order");

            tableModel->setHeaderData(0, Qt::Horizontal, "Номер", Qt::DisplayRole);
            tableModel->setHeaderData(1, Qt::Horizontal, "Дата", Qt::DisplayRole);
            tableModel->setHeaderData(2, Qt::Horizontal, "Мастер", Qt::DisplayRole);
            tableModel->setHeaderData(3, Qt::Horizontal, "Клиент", Qt::DisplayRole);
            tableModel->setHeaderData(4, Qt::Horizontal, "Тип одежды", Qt::DisplayRole);
            tableModel->setHeaderData(5, Qt::Horizontal, "Статус выполнения", Qt::DisplayRole);


            ui->radioButton_searchCompleted->show();
            ui->radioButton_searchUnfulfilled->show();

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
            ui->label_search1->setText("Номер:");
            ui->label_search2->setText("Дата:");
            ui->label_search3->setText("Мастер:");
            ui->label_search4->setText("Клиент:");
            ui->label_search5->setText("Тип одежды:");

            ui->pushButton_searchExport->show();

            QStringList tocNames = DBConnection::ToCNamesList();
            QStringList masterIds = DBConnection::masterIDList();
            QStringList masterTelnums = DBConnection::masterTelnumList();
            QStringList custIds = DBConnection::custIDList();
            QStringList custTelnums = DBConnection::custTelnumList();

            ItemDelegate *itDgEp = new ItemDelegate(emptiness);
            ItemDelegateComboBox *itDgToC = new ItemDelegateComboBox(tocNames, tocNames);
            ItemDelegateComboBox *itDgMaster = new ItemDelegateComboBox(masterIds, masterTelnums);
            ItemDelegateComboBox *itDgCust = new ItemDelegateComboBox(custIds, custTelnums);
            ItemDelegateCheckBox *itDgStat = new ItemDelegateCheckBox();
            //ItemDelegateDate *itDgDate = new ItemDelegateDate("dd.MM.yyyy");

            ui->tableView_search->setItemDelegateForColumn(0, itDgEp);//дату и ID менять нельзя
            ui->tableView_search->setItemDelegateForColumn(1, itDgEp);
            ui->tableView_search->setItemDelegateForColumn(2, itDgMaster);
            ui->tableView_search->setItemDelegateForColumn(3, itDgCust);
            ui->tableView_search->setItemDelegateForColumn(4, itDgToC);
            ui->tableView_search->setItemDelegateForColumn(5, itDgStat);

            ui->tableView_searchHardware->show();
            ui->tableView_searchMaterial->show();
            ui->splitter_searchTables->setSizes(QList<int>() << 1000 << 0);
            ui->splitter_searchTables->handle(1)->setEnabled(true);
        }
        else if(tableName == "Клиент")
        {
            ui->tableView_search->setSortingEnabled(true);

            tableModel->setTable("customer");
            tableModel->setHeaderData(0, Qt::Horizontal, "Номер", Qt::DisplayRole);
            tableModel->setHeaderData(1, Qt::Horizontal, "Имя", Qt::DisplayRole);
            tableModel->setHeaderData(2, Qt::Horizontal, "Фамилия", Qt::DisplayRole);
            tableModel->setHeaderData(3, Qt::Horizontal, "Номер телефона", Qt::DisplayRole);
            tableModel->setHeaderData(4, Qt::Horizontal, "Электронная почта", Qt::DisplayRole);

            ui->radioButton_searchCompleted->hide();
            ui->radioButton_searchUnfulfilled->hide();

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
            ui->label_search1->setText("Номер:");
            ui->label_search2->setText("Имя:");
            ui->label_search3->setText("Фамилия:");
            ui->label_search4->setText("Номер телефона:");
            ui->label_search5->setText("Электронная почта:");

            ui->pushButton_searchExport->hide();

            ItemDelegate *itDgName = new ItemDelegate(rxString45_ru);
            ItemDelegate *itDgNumTel = new ItemDelegate(rxNumTel);
            ItemDelegate *itDgEmail = new ItemDelegate(rxEmail);
            ItemDelegate *itDgEp = new ItemDelegate(emptiness);

            ui->tableView_search->setItemDelegateForColumn(0, itDgEp);
            ui->tableView_search->setItemDelegateForColumn(1, itDgName);
            ui->tableView_search->setItemDelegateForColumn(2, itDgName);
            ui->tableView_search->setItemDelegateForColumn(3, itDgNumTel);
            ui->tableView_search->setItemDelegateForColumn(4, itDgEmail);

            ui->tableView_searchHardware->hide();//спрячем таблички для вывода информации в информационнные таблички поиска
            ui->tableView_searchMaterial->hide();
            ui->splitter_searchTables->setSizes(QList<int>() << 1000 << 0);
            ui->splitter_searchTables->handle(1)->setEnabled(false);
        }
        else if(tableName == "Мастер")
        {
            ui->tableView_search->setSortingEnabled(true);

            tableModel->setTable("master");
            tableModel->setHeaderData(0, Qt::Horizontal, "Номер", Qt::DisplayRole);
            tableModel->setHeaderData(1, Qt::Horizontal, "Имя", Qt::DisplayRole);
            tableModel->setHeaderData(2, Qt::Horizontal, "Фамилия", Qt::DisplayRole);
            tableModel->setHeaderData(3, Qt::Horizontal, "Адрес", Qt::DisplayRole);
            tableModel->setHeaderData(4, Qt::Horizontal, "Номер телефона", Qt::DisplayRole);

            ui->radioButton_searchCompleted->hide();
            ui->radioButton_searchUnfulfilled->hide();

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
            ui->label_search1->setText("Номер:");
            ui->label_search2->setText("Имя:");
            ui->label_search3->setText("Фамилия:");
            ui->label_search4->setText("Адрес:");
            ui->label_search5->setText("Номер телефона:");

            ui->pushButton_searchExport->hide();

            ItemDelegate *itDgName = new ItemDelegate(rxString45_ru);
            ItemDelegate *itDgNumTel = new ItemDelegate(rxNumTel);
            ItemDelegate *itDgAddress = new ItemDelegate(rxStringNum45_eng_ru);
            ItemDelegate *itDgID = new ItemDelegate(emptiness);

            ui->tableView_search->setItemDelegateForColumn(0, itDgID);
            ui->tableView_search->setItemDelegateForColumn(1, itDgName);
            ui->tableView_search->setItemDelegateForColumn(2, itDgName);
            ui->tableView_search->setItemDelegateForColumn(3, itDgAddress);
            ui->tableView_search->setItemDelegateForColumn(4, itDgNumTel);

            ui->tableView_searchHardware->hide();//спрячем таблички для вывода информации в информационнные таблички поиска
            ui->tableView_searchMaterial->hide();
            ui->splitter_searchTables->setSizes(QList<int>() << 1000 << 0);
            ui->splitter_searchTables->handle(1)->setEnabled(false);
        }
        else if(tableName == "Тип одежды")
        {
            ui->tableView_search->setSortingEnabled(true);

            tableModel->setTable("typeofcloth");
            tableModel->setHeaderData(0, Qt::Horizontal, "Наименование", Qt::DisplayRole);
            tableModel->setHeaderData(1, Qt::Horizontal, "Рабочих дней на изготовление", Qt::DisplayRole);
            tableModel->setHeaderData(2, Qt::Horizontal, "Цена", Qt::DisplayRole);

            ui->radioButton_searchCompleted->hide();
            ui->radioButton_searchUnfulfilled->hide();

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

            ui->pushButton_searchExport->hide();

            ItemDelegate *itDgName = new ItemDelegate(rxStringNum45_eng_ru);
            ItemDelegateSpinBox *itDgSb = new ItemDelegateSpinBox(ui->spinBox_addToCDays->minimum(), ui->spinBox_addToCDays->maximum());
            ItemDelegateDoubleSpinBox *itDgDsb = new ItemDelegateDoubleSpinBox(ui->doubleSpinBox_addToCPrice->minimum(), ui->doubleSpinBox_addToCPrice->maximum());

            ui->tableView_search->setItemDelegateForColumn(0, itDgName);
            ui->tableView_search->setItemDelegateForColumn(1, itDgSb);
            ui->tableView_search->setItemDelegateForColumn(2, itDgDsb);

            ui->tableView_searchHardware->hide();//спрячем таблички для вывода информации в информационнные таблички поиска
            ui->tableView_searchMaterial->hide();
            ui->splitter_searchTables->setSizes(QList<int>() << 1000 << 0);
            ui->splitter_searchTables->handle(1)->setEnabled(false);
        }
        else if(tableName == "Фурнитура")
        {
            ui->tableView_search->setSortingEnabled(true);

            tableModel->setTable("hardware");
            tableModel->setHeaderData(0, Qt::Horizontal, "Наименование", Qt::DisplayRole);
            tableModel->setHeaderData(1, Qt::Horizontal, "Цена", Qt::DisplayRole);
            tableModel->setHeaderData(2, Qt::Horizontal, "Количество в наличии", Qt::DisplayRole);

            ui->radioButton_searchCompleted->hide();
            ui->radioButton_searchUnfulfilled->hide();

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

            ui->pushButton_searchExport->hide();

            ItemDelegate *itDgName = new ItemDelegate(rxStringNum45_eng_ru);
            ItemDelegateDoubleSpinBox *itDgDsb = new ItemDelegateDoubleSpinBox(MINSPINBOXVALUE, MAXSPINBOXVALUE);

            ui->tableView_search->setItemDelegateForColumn(0, itDgName);
            ui->tableView_search->setItemDelegateForColumn(1, itDgDsb);
            ui->tableView_search->setItemDelegateForColumn(2, itDgDsb);

            ui->tableView_searchHardware->hide();//спрячем таблички для вывода информации в информационнные таблички поиска
            ui->tableView_searchMaterial->hide();
            ui->splitter_searchTables->setSizes(QList<int>() << 1000 << 0);
            ui->splitter_searchTables->handle(1)->setEnabled(false);
        }
        else if(tableName == "Материал")
        {
            ui->tableView_search->setSortingEnabled(true);

            tableModel->setTable("material");
            tableModel->setHeaderData(0, Qt::Horizontal, "Наименование", Qt::DisplayRole);
            tableModel->setHeaderData(1, Qt::Horizontal, "Цена", Qt::DisplayRole);
            tableModel->setHeaderData(2, Qt::Horizontal, "Количество в наличии", Qt::DisplayRole);

            ui->radioButton_searchCompleted->hide();
            ui->radioButton_searchUnfulfilled->hide();

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

            ui->pushButton_searchExport->hide();

            ItemDelegate *itDgName = new ItemDelegate(rxStringNum45_eng_ru);
            ItemDelegateDoubleSpinBox *itDgDsb = new ItemDelegateDoubleSpinBox(MINSPINBOXVALUE, MAXSPINBOXVALUE);

            ui->tableView_search->setItemDelegateForColumn(0, itDgName);
            ui->tableView_search->setItemDelegateForColumn(1, itDgDsb);
            ui->tableView_search->setItemDelegateForColumn(2, itDgDsb);

            ui->tableView_searchHardware->hide();//спрячем таблички для вывода информации в информационнные таблички поиска
            ui->tableView_searchMaterial->hide();
            ui->splitter_searchTables->setSizes(QList<int>() << 1000 << 0);
            ui->splitter_searchTables->handle(1)->setEnabled(false);
        }
        tableModel->select();//взятие данных из таблиц
        ui->tableView_search->setModel(tableModel);
    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void MainWindow::hideNonValidRowsSearchTable(int column, QString compareText, typeSearch type)//скрываем строки неподходящие по поиску
{
    for(int i(0); i < tableModel->rowCount(); i++)
        if(type == Text)
        {
            if(compareText.contains("<<"))//диапазон
            {
                double from = compareText.section("<<", 0, 0).toDouble();//от 5
                double to = compareText.section("<<", 1, 1).toDouble();//до 10
                if (tableModel->data(tableModel->index(i, column)).toDouble() < from || tableModel->data(tableModel->index(i, column)).toDouble() > to)
                    ui->tableView_search->hideRow(i);
            }
            else if(compareText.contains("<"))//меньше
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
                if(!tableModel->data(tableModel->index(i, column)).toString().contains(compareText, Qt::CaseInsensitive))
                    ui->tableView_search->hideRow(i);
            }
        }
        else if(type == Date)
        {
            if(compareText.contains("<<"))//диапазон
            {
                QDate date = tableModel->data(tableModel->index(i, column)).toDate();
                QDate from = QDate::fromString(compareText.section("<<", 0, 0), "dd.MM.yyyy");
                QDate to = QDate::fromString(compareText.section("<<", 1, 1), "dd.MM.yyyy");
                if(date.isValid() && from.isValid() && to.isValid())
                    if (date < from || date > to)
                        ui->tableView_search->hideRow(i);
            }
            else if(compareText.contains("<"))//меньше
            {
                QString removedText = compareText;
                QDate compareDate = QDate::fromString(removedText.remove("<"), "dd.MM.yyyy");
                QDate date = tableModel->data(tableModel->index(i, column)).toDate();
                if(date.isValid() && compareDate.isValid())
                    if (date > compareDate)
                        ui->tableView_search->hideRow(i);
            }
            else if(compareText.contains(">"))
            {
                QString removedText = compareText;
                QDate compareDate = QDate::fromString(removedText.remove(">"), "dd.MM.yyyy");
                QDate date = tableModel->data(tableModel->index(i, column)).toDate();
                if(date.isValid() && compareDate.isValid())
                    if (date < compareDate)
                        ui->tableView_search->hideRow(i);
            }
            else
            {
                QDate date = QDate::fromString(tableModel->data(tableModel->index(i, column)).toString(), "yyyy-MM-dd");
                if(!date.toString("dd.MM.yyyy").contains(compareText, Qt::CaseInsensitive))
                    ui->tableView_search->hideRow(i);
            }
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
        {
            if(tableModel->tableName() == "order")
                hideNonValidRowsSearchTable(1, ui->lineEdit_search2->text(), Date);
            else
                hideNonValidRowsSearchTable(1, ui->lineEdit_search2->text());
        }

        if(!ui->lineEdit_search3->isHidden())
            hideNonValidRowsSearchTable(2, ui->lineEdit_search3->text());

        if(!ui->lineEdit_search4->isHidden())
            hideNonValidRowsSearchTable(3, ui->lineEdit_search4->text());

        if(!ui->lineEdit_search5->isHidden())
            hideNonValidRowsSearchTable(4, ui->lineEdit_search5->text());

        if(!ui->radioButton_searchCompleted->isHidden())
        {
            if(ui->radioButton_searchCompleted->isChecked() && !ui->radioButton_searchUnfulfilled->isChecked())
                hideNonValidRowsSearchTable(5, "1");
            if(!ui->radioButton_searchCompleted->isChecked() && ui->radioButton_searchUnfulfilled->isChecked())
                hideNonValidRowsSearchTable(5, "0");
        }

    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }

}

void MainWindow::on_lineEdit_search1_textChanged(const QString &/*arg1*/)
{
    updateSearchTable();
}

void MainWindow::on_lineEdit_search2_textChanged(const QString &/*arg1*/)
{
    updateSearchTable();
}

void MainWindow::on_lineEdit_search3_textChanged(const QString &/*arg1*/)
{
    updateSearchTable();
}

void MainWindow::on_lineEdit_search4_textChanged(const QString &/*arg1*/)
{
    updateSearchTable();
}

void MainWindow::on_lineEdit_search5_textChanged(const QString &/*arg1*/)
{
    updateSearchTable();
}

void MainWindow::on_action_addUser_triggered()
{
    signupWindow = new signUpWindow(this);
    signupWindow->show();
}

void MainWindow::on_pushButton_searchDelete_clicked()
{
    try {
        QList<QModelIndex> selected = ui->tableView_search->selectionModel()->selection().indexes();
        QSet<QString> numbersSet;
        QString messNums;

        for(QModelIndex index : selected)
            numbersSet << tableModel->data(tableModel->index(index.row(), 0)).toString();

        QStringList numbers = QStringList::fromSet(numbersSet);

        for(QString num : numbers)
            messNums += num + (num == numbers.last() ? nullptr : ", ");

        QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Выход", "Вы уверены, что хотите удалить " + messNums + "?", QMessageBox::No | QMessageBox::Yes);
        if (resBtn == QMessageBox::Yes)
        {
            for(QModelIndex index : selected)
                ui->tableView_search->model()->removeRow(index.row());
            tableModel->select();
        }
    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void MainWindow::on_tableView_search_clicked(const QModelIndex &index)
{
    if(tableModel->tableName() == "order")
    {
        if(ui->splitter_searchTables->sizes()[1] == 0)
        {
            int size = ui->splitter_searchTables->size().width();
            ui->splitter_searchTables->setSizes(QList<int>() << size/4*3 << size/4);
        }
        QString idOrder = ui->tableView_search->model()->data(ui->tableView_search->model()->index(index.row(), 0)).toString();
        tabModMat = DBConnection::tmMaterialList(idOrder);
        tabModHar = DBConnection::tmHardwareList(idOrder);
        ui->tableView_searchMaterial->setModel(tabModMat);
        ui->tableView_searchHardware->setModel(tabModHar);
        QStringList matList = DBConnection::materialList();
        QStringList harList = DBConnection::hardwareList();
        ItemDelegateComboBox *itDgMat = new ItemDelegateComboBox(matList, matList);
        ItemDelegateComboBox *itDgHar = new ItemDelegateComboBox(harList, harList);
        ui->tableView_searchMaterial->setItemDelegateForColumn(2, itDgMat);
        ui->tableView_searchHardware->setItemDelegateForColumn(2, itDgHar);
        ui->tableView_searchMaterial->hideColumn(3);//прячем айди заказа
        ui->tableView_searchHardware->hideColumn(3);
        ui->tableView_searchMaterial->hideColumn(0);//прячем айди самой записи
        ui->tableView_searchHardware->hideColumn(0);
    }
}

void MainWindow::on_pushButton_searchDelMat_clicked()
{
    try {
        tabModMat->removeRow(ui->tableView_searchMaterial->currentIndex().row());
        tabModMat->select();
    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void MainWindow::on_pushButton_searchDelHar_clicked()
{
    try {
        tabModHar->removeRow(ui->tableView_searchHardware->currentIndex().row());
        tabModHar->select();
    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void MainWindow::on_pushButton_searchAddMat_clicked()
{
    try {
        QString idOrder = ui->tableView_search->model()->data(ui->tableView_search->model()->index(ui->tableView_search->currentIndex().row(), 0)).toString();
        DBConnection::addMaterialInOrder(idOrder);
        tabModMat->select();
    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void MainWindow::on_pushButton_searchAddHar_clicked()
{
    try {
        QString idOrder = ui->tableView_search->model()->data(ui->tableView_search->model()->index(ui->tableView_search->currentIndex().row(), 0)).toString();
        DBConnection::addHardwareInOrder(idOrder);
        tabModHar->select();
    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void MainWindow::on_pushButton_searchExport_clicked()
{
    try {
        QList<QModelIndex> selected = ui->tableView_search->selectionModel()->selection().indexes();
        QSet<QString> ids;

        for(QModelIndex index : selected)
        {
            QModelIndex currRow = tableModel->index(index.row(), 0);
            QString data = ui->tableView_search->model()->data(currRow).toString();
            ids << data;
        }

        for(QString id : ids)
        {
            order.write(id);
            QMessageBox::information(this, "Успех!", "Файл заказа находится по пути: " + order.lastPath());
        }
    } catch (const std::exception& e) {
        Log::write(e.what());
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void MainWindow::on_radioButton_searchCompleted_clicked()
{
    updateSearchTable();
}

void MainWindow::on_radioButton_searchUnfulfilled_clicked()
{
    updateSearchTable();
}

void MainWindow::on_action_statistic_triggered()
{
    statsWindow = new StatisticWindow(StatsProp::Profit);
    statsWindow->show();
}

void MainWindow::on_action_popularity_triggered()
{
    statsWindow = new StatisticWindow(StatsProp::AllTypeOfClothPopularity);
    statsWindow->show();
}

void MainWindow::on_action_searchCust_triggered()
{
    for (int i(0); ui->tabWidget->count(); i++)
        if(ui->tabWidget->tabText(i) == "Поиск")
        {
            ui->tabWidget->setCurrentIndex(i);
            break;
        }
    ui->comboBox_searchTableName->setCurrentIndex(1);
    on_comboBox_searchTableName_activated("Клиент");
}

void MainWindow::on_action_searchOrder_triggered()
{
    for (int i(0); ui->tabWidget->count(); i++)
        if(ui->tabWidget->tabText(i) == "Поиск")
        {
            ui->tabWidget->setCurrentIndex(i);
            break;
        }
    ui->comboBox_searchTableName->setCurrentIndex(0);
    on_comboBox_searchTableName_activated("Заказ");
}

void MainWindow::on_action_searchMaterial_triggered()
{
    for (int i(0); ui->tabWidget->count(); i++)
        if(ui->tabWidget->tabText(i) == "Поиск")
        {
            ui->tabWidget->setCurrentIndex(i);
            break;
        }
    ui->comboBox_searchTableName->setCurrentIndex(5);
    on_comboBox_searchTableName_activated("Материал");
}

void MainWindow::on_action_searchHardware_triggered()
{
    for (int i(0); ui->tabWidget->count(); i++)
        if(ui->tabWidget->tabText(i) == "Поиск")
        {
            ui->tabWidget->setCurrentIndex(i);
            break;
        }
    ui->comboBox_searchTableName->setCurrentIndex(4);
    on_comboBox_searchTableName_activated("Фурнитура");
}

void MainWindow::on_action_searchtoc_triggered()
{
    for (int i(0); ui->tabWidget->count(); i++)
        if(ui->tabWidget->tabText(i) == "Поиск")
        {
            ui->tabWidget->setCurrentIndex(i);
            break;
        }
    ui->comboBox_searchTableName->setCurrentIndex(3);
    on_comboBox_searchTableName_activated("Тип одежды");
}

void MainWindow::on_action_searchMaster_triggered()
{
    for (int i(0); ui->tabWidget->count(); i++)
        if(ui->tabWidget->tabText(i) == "Поиск")
        {
            ui->tabWidget->setCurrentIndex(i);
            break;
        }
    ui->comboBox_searchTableName->setCurrentIndex(2);
    on_comboBox_searchTableName_activated("Мастер");
}
