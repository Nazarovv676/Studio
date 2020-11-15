#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include "log.h"
#include "settings.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <exception>

class DBConnection
{
public:
    static QSqlDatabase database;
    static QString host;
    static QString dbName;
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

    static QSqlQuery qToCList();
    static QStringList ToCNamesList();

    static QSqlQuery qMaterialNameList();
    static QStringList materialList();
    static QString materialList(QString orderID);
    static QSqlTableModel *tmMaterialList(QString orderID);

    static QSqlQuery qHardwareNameList();
    static QStringList hardwareList();
    static QString hardwareList(QString orderID);
    static QSqlTableModel *tmHardwareList(QString orderID);


    static void addMaterialInOrder(const QString &order);
    static void addHardwareInOrder(const QString &order);

    static void addCustomer(const QString &name, const QString &surname, const QString &numTel, const QString &email);
    static bool containsCust(const QString &telNum);
    static bool containsCust(const int &ID);
    static QString custID(const QString &telNum);
    static QString custIDByOrder(QString orderID);
    static QString custIdByNumTel(const QString &numTel);
    static QString custNameByID(QString ID);
    static QStringList custIDList();
    static QStringList custTelnumList();

    static void addMaster(const QString &name, const QString &surname, const QString &address, const QString &numTel);
    static bool containsMaster(const QString &telNum);
    static bool containsMaster(const int &ID);
    static QString masterID(const QString &telNum);
    static QString masterIdByNumTel(const QString &numTel);
    static QString masterIDByOrder(QString orderID);
    static QString masterNameByID(QString ID);
    static QSqlQuery qMasterList();
    static QStringList masterIDList();
    static QStringList masterTelnumList();

    static void addHardware(const QString &name, const QString &price, const QString &quantity);
    static bool containsHardware(const QString &name);

    static void addMaterial(const QString &name, const QString &price, const QString &quantity);
    static bool containsMaterial(const QString &name);

    static void addTypeOfCloth(const QString &name, const QString &price, const QString &days);
    static bool containsTypeOfCloth(const QString &name);
    static QString typeOfClothByOrder(QString orderID);

    static QString addOrder(const QString &custID, const QString &masterID, const QString &ToCName, const QStringList &materialName, const QStringList &materialQuant, const QStringList &hardName, const QStringList &hardQuant);
    static QString orderPrice(QString orderID);
    static double orderProfit(QString orderID);
    static QList<double> orderProfit(QDate from, QDate to);
    static QList<QDate> orderDates(QDate from, QDate to);
    static QList<double> ordersProfit(QList<QDate> dates);
    static double ordersProfit(QDate date);
};

#endif // DBCONNECTION_H
