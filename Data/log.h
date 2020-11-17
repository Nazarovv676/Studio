#ifndef LOG_H
#define LOG_H
#include <QFile>
#include <exception>
#include <QTextStream>
#include <QDateTime>

class Log
{
    static QFile file;
public:
    static void open();
    static void write(QString mess);
};

#endif // LOG_H
