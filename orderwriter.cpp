#include "orderwriter.h"
#include "dbconnection.h"
#include "log.h"

#include <QtPrintSupport/QPrinter>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDate>
#include <QTextDocument>


#include <QDebug>

QString OrderWriter::data() const
{
    return _data;
}

void OrderWriter::setData(const QString &value)
{
    _data = value;
}

QString OrderWriter::fileName() const
{
    return _fileName;
}

void OrderWriter::setFileName(const QString &fileName)
{
    _fileName = fileName;
}

bool OrderWriter::write(QString orderID)
{
    QString custID = DBConnection::custIDByOrder(orderID);
    qDebug() << "CustID = " + custID;

    QString custName = DBConnection::custNameByID(custID);
    qDebug() << "custName = " + custName;

    QString typeOfCloth = DBConnection::typeOfClothByOrder(orderID);
    qDebug() << "typeOfCloth = " + typeOfCloth;

    QString materials = DBConnection::materialList(orderID);
    qDebug() << "materials = " + materials;

    QString hardware = DBConnection::hardwareList(orderID);
    qDebug() << "hardware = " + hardware;

    QString masterName = DBConnection::masterNameByID(DBConnection::masterIDByOrder(orderID));
    qDebug() << "masterName = " + masterName;

    QString price = DBConnection::orderPrice(orderID);
    QString curDate = QDate::currentDate().toString("dd.MM.yyyy");
    _data = "<h1 align = center>"
            "ЗАКАЗ №" + orderID +
        "</h1>"
        "<p>"
            "НОМЕР КЛИЕНТА: " + custID + "<br>"
            "ИМЯ КЛИЕНТА: " + custName + "<br>"
            "<br>"
            "ВИД ИЗДЕЛИЯ: " + typeOfCloth + "<br>"
            "МАТЕРИАЛЫ: " + materials + "<br>"
            "ФУРНИТУРА: " + hardware + "<br>"
            "<br>"
            "МАСТЕР: " + masterName + "<br>"
            "<br>"
            "ЦЕНА: " + price + " грн.<br>"
            "<br>"
            "<br>"
            "ДАТА: " + curDate + "<br>"
        "</p>";

    QTextDocument document;
    document.setHtml(_data);

    QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Документы заказов/";
    QDir dir;
    dir.mkpath(path);

    QFile file(path + orderID + ".pdf");
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
        return false;
    file.close();

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName(file.fileName());
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));

    document.print(&printer);
    return true;
}

OrderWriter::OrderWriter()
{

}
