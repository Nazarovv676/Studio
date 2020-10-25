#include "log.h"
#include <QDebug>

void Log::open()
{
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
        throw std::runtime_error("Log is not open");
     file.readAll();
}

void Log::write(QString mess)
{
    QTextStream out(&file);
    out << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss ") << mess << '\n';
}
