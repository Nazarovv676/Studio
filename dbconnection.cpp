#include "dbconnection.h"
#include <QString>

#include <QDebug>

//-------------------------methods-----------------------

void DBConnection::connect()
{
    database.setHostName(host);
    database.setDatabaseName(dbName);
    database.setUserName(userName);
    database.setPassword(userPass);
    database.setPort(port.toInt());
    if(!database.open())
    {
        qDebug() << database.lastError().nativeErrorCode() << database.lastError();
        throw std::runtime_error(database.lastError().nativeErrorCode().toStdString());
    }
}

int DBConnection::customersCount()
{
    if(!query.exec("SELECT MAX(id) FROM studio.customer"))
        throw std::runtime_error(query.lastError().text().toStdString());
    query.next();
    return query.value(0).toInt();
}

int DBConnection::masterCount()
{
    if(!query.exec("SELECT MAX(id) FROM studio.master"))
        throw std::runtime_error(query.lastError().text().toStdString());
    query.next();
    return query.value(0).toInt();
}

int DBConnection::orderCount()
{
    //query.exec("ANALYZE TABLE `order`");
    if(!query.exec("SELECT MAX(id) FROM studio.order"))
        throw std::runtime_error(query.lastError().text().toStdString());
    query.next();
    return query.value(0).toInt();
}

void DBConnection::addUser(const QString &name, const QString &pass)
{
    if (!query.exec("CREATE USER '" + name + "'@'" + host + "' IDENTIFIED WITH mysql_native_password BY '" + pass + "';"))
        throw std::runtime_error(query.lastError().text().toStdString());
    if (!query.exec("GRANT SELECT, INSERT, TRIGGER, UPDATE, DELETE ON TABLE `" + Settings::settings.value("CONNECTION/DBNAME").toString() + "`.* TO '" + name + "'@'" + host + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    if (!query.exec("GRANT EXECUTE ON " + Settings::settings.value("CONNECTION/DBNAME").toString() + ".* TO '" + name + "'@'" + host + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
}

QSqlQuery DBConnection::qToCList()
{
    if(!query.exec("SELECT name FROM studio.typeofcloth"))
        throw std::runtime_error(query.lastError().text().toStdString());
    return query;
}

QSqlQuery DBConnection::qMaterialNameList()
{
    if(!query.exec("SELECT name FROM studio.material"))
        throw std::runtime_error(query.lastError().text().toStdString());
    return query;
}

QSqlQuery DBConnection::qHardwareNameList()
{
    if(!query.exec("SELECT name FROM studio.hardware"))
        throw std::runtime_error(query.lastError().text().toStdString());
    return query;
}

QSqlQuery DBConnection::qMasterList()
{
    if(!query.exec("SELECT * FROM studio.master"))
        throw std::runtime_error(query.lastError().text().toStdString());
    return query;
}

QStringList DBConnection::ToCNamesList()
{
    if(!query.exec("SELECT name FROM typeofcloth"))
        throw std::runtime_error(query.lastError().text().toStdString());
    QStringList list;
    while (query.next())
        list << query.value(0).toString();
    return list;
}

QStringList DBConnection::masterIDList()
{
    if(!query.exec("SELECT id FROM master"))
        throw std::runtime_error(query.lastError().text().toStdString());
    QStringList list;
    while (query.next())
        list << query.value(0).toString();
    return list;
}

QStringList DBConnection::masterTelnumList()
{
    if(!query.exec("SELECT telnum FROM master"))
        throw std::runtime_error(query.lastError().text().toStdString());
    QStringList list;
    while (query.next())
        list << query.value(0).toString();
    return list;
}

QStringList DBConnection::custIDList()
{
    if(!query.exec("SELECT id FROM customer"))
        throw std::runtime_error(query.lastError().text().toStdString());
    QStringList list;
    while (query.next())
        list << query.value(0).toString();
    return list;
}

QStringList DBConnection::custTelnumList()
{
    if(!query.exec("SELECT telnum FROM customer"))
        throw std::runtime_error(query.lastError().text().toStdString());
    QStringList list;
    while (query.next())
        list << query.value(0).toString();
    return list;
}

QStringList DBConnection::materialList()
{
    if(!query.exec("SELECT name FROM material"))
        throw std::runtime_error(query.lastError().text().toStdString());
    QStringList list;
    while (query.next())
        list << query.value(0).toString();
    return list;
}

QString DBConnection::materialList(QString orderID)
{
    QSqlTableModel *tm = tmMaterialList(orderID);
    QString list;

    for(int i(0); i < tm->rowCount(QModelIndex()); i++)
        list += tm->data(tm->index(i, 2)).toString() + " - " + tm->data(tm->index(i, 1)).toString() + "; ";
    return list;
}

QString DBConnection::hardwareList(QString orderID)
{
    QSqlTableModel *tm = tmHardwareList(orderID);
    QString list;

    for(int i(0); i < tm->rowCount(QModelIndex()); i++)
        list += tm->data(tm->index(i, 2)).toString() + " - " + tm->data(tm->index(i, 1)).toString() + "; ";
    return list;
}

QStringList DBConnection::hardwareList()
{
    if(!query.exec("SELECT name FROM hardware"))
        throw std::runtime_error(query.lastError().text().toStdString());
    QStringList list;
    while (query.next())
        list << query.value(0).toString();
    return list;
}

QSqlTableModel *DBConnection::tmMaterialList(QString orderID)
{
    QSqlTableModel *tm = new QSqlTableModel(nullptr, database);
    tm->setTable("material_order");
    tm->setFilter("order_id = " + orderID);
    tm->select();
    tm->setHeaderData(1, Qt::Horizontal, "Количество", Qt::DisplayRole);
    tm->setHeaderData(2, Qt::Horizontal, "Наименование", Qt::DisplayRole);
    tm->setEditStrategy(QSqlTableModel::OnFieldChange);
    return tm;
}

QSqlTableModel *DBConnection::tmHardwareList(QString orderID)
{
    QSqlTableModel *tm = new QSqlTableModel(nullptr, database);
    tm->setTable("hardware_order");
    tm->setFilter("order_id = " + orderID);
    tm->select();
    tm->setHeaderData(1, Qt::Horizontal, "Количество", Qt::DisplayRole);
    tm->setHeaderData(2, Qt::Horizontal, "Наименование", Qt::DisplayRole);
    tm->setEditStrategy(QSqlTableModel::OnFieldChange);
    return tm;
}

QString DBConnection::custIDByOrder(QString orderID)
{
    if(!query.exec("SELECT customer_id FROM studio.order where id = '" + orderID + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    query.next();
    return query.value(0).toString();
}

void DBConnection::addMaterialInOrder(const QString &order)
{
    if(materialList().length() > 0)
        if(!query.exec("INSERT INTO material_order (quantity, material_name, order_id) VALUES ('0', '" + materialList()[0] + "', '" + order + "')"))
            throw std::runtime_error(query.lastError().text().toStdString());
}

void DBConnection::addHardwareInOrder(const QString &order)
{
    if(hardwareList().length() > 0)
        if(!query.exec("INSERT INTO hardware_order (quantity, hardware_name, order_id) VALUES ('0', '" + hardwareList()[0] + "', '" + order + "')"))
            throw std::runtime_error(query.lastError().text().toStdString());
}

QString DBConnection::masterIdByNumTel(const QString &numTel)
{
    if(!query.exec("SELECT id FROM studio.master WHERE telnum = '" + numTel + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    if(query.next())
        return query.value(0).toString();
    else
        return NULL;
}

QString DBConnection::masterIDByOrder(QString orderID)
{
    if(!query.exec("SELECT master_id FROM studio.order WHERE id = '" + orderID + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    query.next();
    return query.value(0).toString();
}

QString DBConnection::masterNameByID(QString ID)
{
    if(!query.exec("SELECT name, surname FROM master WHERE id = '" + ID + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    query.next();
    return query.value(0).toString() + " " + query.value(1).toString();
}

QString DBConnection::custIdByNumTel(const QString &numTel)
{
    if(!query.exec("SELECT id FROM studio.customer WHERE telnum = '" + numTel + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    if(query.next())
        return query.value(0).toString();
    else
        return NULL;
}

QString DBConnection::custNameByID(QString ID)
{
    if(!query.exec("SELECT name, surname FROM customer WHERE id = '" + ID + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    query.next();
    return query.value(0).toString() + " " + query.value(1).toString();
}

void DBConnection::addCustomer(const QString &name, const QString &surname, const QString &numTel, const QString &email)
{
    if(!query.exec("INSERT INTO `studio`.`customer` (`id`, `name`, `surname`, `telnum`, `mail`) VALUES ('" + QString::number(customersCount() + 1) + "', '" + name + "', '" + surname + "', '" + numTel + "', '" + email + "')"))
        throw std::runtime_error(query.lastError().text().toStdString());
}

bool DBConnection::containsCust(const QString &telNum)
{
    if(!query.exec("select id from `studio`.`customer` where telnum = '" + telNum + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    if(query.next())
        return true;
    return false;
}

bool DBConnection::containsCust(const int &ID)
{
    if(!query.exec("select id from `studio`.`customer` where id = '" + QString::number(ID) + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    return query.next();
}

QString DBConnection::custID(const QString &telNum)
{
    if(!query.exec("select id from `studio`.`customer` where telnum = '" + telNum + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    query.next();
    return query.value(0).toString();
}

void DBConnection::addMaster(const QString &name, const QString &surname, const QString &address, const QString &numTel)
{
    //INSERT INTO `studio`.`master` (`name`, `surname`, `address`, `telnum`) VALUES ('Ольга', 'Кравцова', 'Титова 22а', '+380-63-934-45-12');
    if(!query.exec("INSERT INTO `studio`.`master` (`id`, `name`, `surname`, `address`, `telnum`) VALUES ('" + QString::number(masterCount() + 1) + "', '" + name + "', '" + surname + "', '" + address + "', '" + numTel + "')"))
        throw std::runtime_error(query.lastError().text().toStdString());
}

bool DBConnection::containsMaster(const QString &telNum)
{
    if(!query.exec("select id from `studio`.`master` where telnum = '" + telNum + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    if(query.next())
        return true;
    return false;
}

bool DBConnection::containsMaster(const int &ID)
{
    if(!query.exec("select id from `studio`.`master` where id = '" + QString::number(ID) + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    return query.next();
}

QString DBConnection::masterID(const QString &telNum)
{
    if(!query.exec("select id from `studio`.`master` where telnum = '" + telNum + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    query.next();
    return query.value(0).toString();
}

bool DBConnection::containsHardware(const QString &name)
{
    if(!query.exec("select * from `studio`.`hardware` where name = '" + name + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    if(query.next())
        return true;
    return false;
}

bool DBConnection::containsMaterial(const QString &name)
{
    if(!query.exec("select * from `studio`.`material` where name = '" + name + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    if(query.next())
        return true;
    return false;
}

void DBConnection::addTypeOfCloth(const QString &name, const QString &price, const QString &days)
{
    QString PRICE(price);
    PRICE.replace(",", ".");
    qDebug() << "addTypeOfCloth()" << name << price << days;
    //INSERT INTO `studio`.`typeofcloth` (`name`, `days`, `price`) VALUES ('Dress Chanel', '15', '15000.25');
    if(!containsTypeOfCloth(name))
    {
        if(!query.exec("INSERT INTO `studio`.`typeofcloth` (`name`, `days`, `price`) VALUES ('" + name + "', '" + days + "', '" + PRICE + "')"))
            throw std::runtime_error(query.lastError().text().toStdString());
    }
    else if(!query.exec("UPDATE studio.typeofcloth SET days = " + days +", price = " + PRICE + " WHERE name = '" + name + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
}

bool DBConnection::containsTypeOfCloth(const QString &name)
{
    //SELECT * FROM studio.typeofcloth WHERE name = 'Dress Chanel';
    if(!query.exec("SELECT * FROM studio.typeofcloth WHERE name = '" + name + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    if(query.next())
        return true;
    return false;
}

QString DBConnection::typeOfClothByOrder(QString orderID)
{
    if(!query.exec("SELECT typeofcloth_name FROM studio.order WHERE id = '" + orderID + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    query.next();
    return query.value(0).toString();
}

void DBConnection::addMaterial(const QString &name, const QString &price, const QString &quantity)
{
    QString PRICE(price);
    PRICE.replace(",", ".");

    if(!containsMaterial(name))
    {
        if(!query.exec("INSERT INTO `studio`.`material` (`name`, `price`, `quantity`) VALUES ('" + name + "', '" + PRICE + "', '" + quantity + "')"))
            throw std::runtime_error(query.lastError().text().toStdString());
    }
    else if(!query.exec("UPDATE studio.material SET quantity = quantity + " + quantity + ", price = " + PRICE + " WHERE name = '" + name + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
}

void DBConnection::addHardware(const QString &name, const QString &price, const QString &quantity)
{
    QString PRICE(price);
    PRICE.replace(",", ".");

    if(!containsHardware(name))
    {
        if(!query.exec("INSERT INTO `studio`.`hardware` (`name`, `price`, `quantity`) VALUES ('" + name + "', '" + PRICE + "', '" + quantity + "')"))
            throw std::runtime_error(query.lastError().text().toStdString());
    }
    else if(!query.exec("UPDATE studio.hardware SET quantity = quantity + " + quantity + ", price = " + PRICE + " WHERE name = '" + name + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    //UPDATE studio.hardware SET quantity = quantity + 1, price =  10.10  WHERE name = 'Зеленая пуговица'
}

QString DBConnection::addOrder(const QString &custID, const QString &masterID, const QString &ToCName, const QStringList &materialName, const QStringList &materialQuant, const QStringList &hardName, const QStringList &hardQuant)
{
    QString orderID = QString::number(orderCount() + 1);

    if(!query.exec("INSERT INTO `studio`.`order` (`id`, `date`, `master_id`, `customer_id`, `typeofcloth_name`) VALUES ('" + orderID + "', '" + QDate::currentDate().toString("yyyy-MM-dd") + "', '" + masterID + "', '" + custID + "', '" + ToCName + "')"))
        throw std::runtime_error(query.lastError().text().toStdString());

    for(int i(0); i < materialName.count(); i++)
        if(!query.exec("INSERT INTO `studio`.`material_order` (`quantity`, `order_id`, `material_name`) VALUES ('" + materialQuant[i] + "', '" + orderID + "', '" + materialName[i] + "')"))
            throw std::runtime_error(query.lastError().text().toStdString());
    for(int i(0); i < hardName.count(); i++)
        if(!query.exec("INSERT INTO `studio`.`hardware_order` (`quantity`, `order_id`, `hardware_name`) VALUES ('" + hardQuant[i] + "', '" + orderID + "', '" + hardName[i] + "')"))
            throw std::runtime_error(query.lastError().text().toStdString());

    return orderID;
}

QString DBConnection::orderPrice(QString orderID)
{
    if(!query.exec("CALL orderPrice('" + orderID + "')"))
        throw std::runtime_error(query.lastError().text().toStdString());
    query.next();
    return query.value(0).toString();
}


