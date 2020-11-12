#ifndef ORDERWRITER_H
#define ORDERWRITER_H

#include <QString>

class OrderWriter
{
    QString _data;
    QString _fileName;
public:
    OrderWriter();

    QString data() const;
    void setData(const QString &value);
    QString fileName() const;
    void setFileName(const QString &fileName);
    bool write(QString orderNum);
};

#endif // ORDERWRITER_H
