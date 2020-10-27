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
        throw std::runtime_error(database.lastError().nativeErrorCode().toStdString());
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

QSqlQuery DBConnection::ToCList()
{
    if(!query.exec("SELECT name FROM studio.typeofcloth"))
        throw std::runtime_error(query.lastError().text().toStdString());
    return query;
}

QSqlQuery DBConnection::materialList()
{
    if(!query.exec("SELECT name FROM studio.material"))
        throw std::runtime_error(query.lastError().text().toStdString());
    return query;
}

QSqlQuery DBConnection::hardwareList()
{
    if(!query.exec("SELECT name FROM studio.hardware"))
        throw std::runtime_error(query.lastError().text().toStdString());
    return query;
}

QString DBConnection::getMasterIdByNumTel(const QString &numTel)
{
    if(!query.exec("SELECT id FROM studio.master WHERE telnum = '" + numTel + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    if(query.next())
        return query.value(0).toString();
    else
        return NULL;
}

QString DBConnection::getCustIdByNumTel(const QString &numTel)
{
    if(!query.exec("SELECT id FROM studio.customer WHERE telnum = '" + numTel + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    if(query.next())
        return query.value(0).toString();
    else
        return NULL;
}

void DBConnection::addCustomer(const QString &name, const QString &surname, const QString &numTel, const QString &email)
{
    if(!query.exec("INSERT INTO `studio`.`customer` (`id`, `name`, `surname`, `telnum`, `mail`) VALUES ('" + QString::number(customersCount() + 1) + "', '" + name + "', '" + surname + "', '" + numTel + "', '" + email + "')"))
        throw std::runtime_error(query.lastError().text().toStdString());
}

bool DBConnection::containsClient(const QString &telNum)
{
    if(!query.exec("select id from `studio`.`customer` where telnum = '" + telNum + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    if(query.next())
        return true;
    return false;
}

bool DBConnection::containsClient(const int &ID)
{
    if(!query.exec("select id from `studio`.`customer` where id = '" + QString::number(ID) + "'"))
        throw std::runtime_error(query.lastError().text().toStdString());
    return query.next();
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

void DBConnection::addOrder(const QString &custID, const QString &masterID, const QString &ToCName, const QStringList &materialName, const QStringList &materialQuant, const QStringList &hardName, const QStringList &hardQuant)
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

    //INSERT INTO `studio`.`material_order` (`quantity`, `order_id`, `material_name`) VALUES ('1', '1', 'Нитки синие');
    //INSERT INTO `studio`.`hardware_order` (`quantity`, `order_id`, `hardware_name`) VALUES ('1', '1', 'Пуговица синяя ');


}


