#ifndef ORDERWRITER_H
#define ORDERWRITER_H

#include <QString>
#include <QFile>

class OrderWriter
{
public:
    OrderWriter();

    QString custID() const;
    QString custName() const;
    QString typeOfCloth() const;
    QString materials() const;
    QString hardware() const;
    QString masterName() const;
    QString price() const;
    QString lastPath();

    virtual bool write(QString orderNum);

protected:
    QString _data;
    QString _custID;
    QString _custName;
    QString _typeOfCloth;
    QString _materials;
    QString _hardware;
    QString _masterName;
    QString _price;
    QFile *_file;

    QString data() const;
    void setData(const QString &value);
};

#endif // ORDERWRITER_H
