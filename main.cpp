#include "loginwindow.h"
#include "dbconnection.h"
#include "log.h"
#include "settings.h"

#include <QApplication>
#include <QStyleFactory>
#include <QTranslator>
#include <QLibraryInfo>

//_________static_________________
QSettings Settings::settings("conf", QSettings::IniFormat);
QPalette Settings::greyPalette("grey");//цвет этой палитры задается вручную методом в конструкторе LoginWindow
QPalette Settings::whitePalette;
QPalette Settings::blackPalette("black");//эту палитру создает qt
//__________________________________________________________

QFile Log::file("log.txt");
//__________________________________________________________

QSqlDatabase DBConnection::database = QSqlDatabase::addDatabase("QMYSQL");
QString DBConnection::host = Settings::settings.value("CONNECTION/HOST").toString();
QString DBConnection::dbName = Settings::settings.value("CONNECTION/DBNAME").toString();
QString DBConnection::port = Settings::settings.value("CONNECTION/PORT").toString();
QString DBConnection::userName;
QString DBConnection::userPass;
QSqlQuery DBConnection::query = QSqlQuery(DBConnection::database);
//________________________________________________________________

int main(int argc, char *argv[])
{
    qApp->setStyle(QStyleFactory::create("Fusion"));
    QApplication a(argc, argv);
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);
    LoginWindow w;
    w.show();
    return a.exec();
}
