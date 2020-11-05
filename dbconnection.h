#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include "log.h"
#include "settings.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <exception>

class DBConnection
{
public:
    static QSqlDatabase database;

    static QString host;
    static QString dbName;//имя базы данных
    static QString userName;
    static QString userPass;
    static QString port;
    static QSqlQuery query;

    //------------------------------------------------

    static void connect();
    static int customersCount();
    static int masterCount();
    static int orderCount();

    static void addUser(const QString &name, const QString &pass);

    static QSqlQuery ToCList();
    static QSqlQuery materialList();
    static QSqlQuery hardwareList();
    static QSqlQuery masterList();

    static QString getMasterIdByNumTel(const QString &numTel);
    static QString getCustIdByNumTel(const QString &numTel);

    static void addCustomer(const QString &name, const QString &surname, const QString &numTel, const QString &email);
    static bool containsClient(const QString &telNum);
    static bool containsClient(const int &ID);
    static QString custID(const QString &telNum);

    static void addMaster(const QString &name, const QString &surname, const QString &address, const QString &numTel);
    static bool containsMaster(const QString &telNum);
    static bool containsMaster(const int &ID);
    static QString masterID(const QString &telNum);

    static void addHardware(const QString &name, const QString &price, const QString &quantity);
    static bool containsHardware(const QString &name);

    static void addMaterial(const QString &name, const QString &price, const QString &quantity);
    static bool containsMaterial(const QString &name);

    static void addTypeOfCloth(const QString &name, const QString &price, const QString &days);
    static bool containsTypeOfCloth(const QString &name);

    static QString addOrder(const QString &custID, const QString &masterID, const QString &ToCName, const QStringList &materialName, const QStringList &materialQuant, const QStringList &hardName, const QStringList &hardQuant);
};

#endif // DBCONNECTION_H
