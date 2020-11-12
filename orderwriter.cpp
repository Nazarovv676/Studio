#include "orderwriter.h"
#include "dbconnection.h"
#include "log.h"

#include <QtPrintSupport/QPrinter>
#include <QDir>
#include <QStandardPaths>
#include <QDate>
#include <QTextDocument>
#include <exception>

#include <QDebug>

OrderWriter::OrderWriter() : _file(nullptr)
{

}

QString OrderWriter::data() const
{
    return _data;
}

void OrderWriter::setData(const QString &value)
{
    _data = value;
}

bool OrderWriter::write(QString orderID)
{
    _custID = DBConnection::custIDByOrder(orderID);
    _custName = DBConnection::custNameByID(_custID);
    _typeOfCloth = DBConnection::typeOfClothByOrder(orderID);
    _materials = DBConnection::materialList(orderID);
    _hardware = DBConnection::hardwareList(orderID);
    _masterName = DBConnection::masterNameByID(DBConnection::masterIDByOrder(orderID));
    _price = DBConnection::orderPrice(orderID);
    QString curDate = QDate::currentDate().toString("dd.MM.yyyy");
    _data = "<h1 align = center>"
            "ЗАКАЗ №" + orderID +
        "</h1>"
        "<p>"
            "НОМЕР КЛИЕНТА: " + _custID + "<br>"
            "ИМЯ КЛИЕНТА: " + _custName + "<br>"
            "<br>"
            "ВИД ИЗДЕЛИЯ: " + _typeOfCloth + "<br>"
            "МАТЕРИАЛЫ: " + _materials + "<br>"
            "ФУРНИТУРА: " + _hardware + "<br>"
            "<br>"
            "МАСТЕР: " + _masterName + "<br>"
            "<br>"
            "ЦЕНА: " + _price + " грн.<br>"
            "<br>"
            "<br>"
            "ДАТА: " + curDate + "<br>"
        "</p>";

    QTextDocument document;
    document.setHtml(_data);

    QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Документы заказов/";
    QDir dir;
    dir.mkpath(path);

    _file = new QFile(path + orderID + ".pdf");
    if(!_file->open(QIODevice::ReadWrite | QIODevice::Text))
        return false;
    _file->close();

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName(_file->fileName());
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));

    document.print(&printer);
    return true;
}

QString OrderWriter::custID() const
{
    return _custID;
}

QString OrderWriter::custName() const
{
    return _custName;
}

QString OrderWriter::typeOfCloth() const
{
    return _typeOfCloth;
}

QString OrderWriter::materials() const
{
    return _materials;
}

QString OrderWriter::hardware() const
{
    return _hardware;
}

QString OrderWriter::masterName() const
{
    return _masterName;
}

QString OrderWriter::price() const
{
    return _price;
}

QString OrderWriter::lastPath()
{
    if(_file != nullptr)
        return _file->fileName();
    throw std::bad_typeid();
}
