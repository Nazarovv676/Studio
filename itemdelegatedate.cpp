#include "itemdelegatedate.h"
#include <QDateEdit>

ItemDelegateDate::ItemDelegateDate(QString dateFormat, bool editable, QObject *parent) : QItemDelegate(parent), _dateFormat(dateFormat), _editable(editable)
{

}

QString ItemDelegateDate::dateFormat() const
{
    return _dateFormat;
}

bool ItemDelegateDate::editable() const
{
    return _editable;
}

QWidget *ItemDelegateDate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    QDateEdit *editor = new QDateEdit(parent);
    editor->setEnabled(_editable);
    return editor;
}

void ItemDelegateDate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QDate date = index.model()->data(index, Qt::EditRole).toDate();
    QDateEdit *dateEdit = static_cast<QDateEdit*>(editor);
    dateEdit->setDate(date);
}

void ItemDelegateDate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDateEdit *dateEdit = static_cast<QDateEdit*>(editor);
    QDate date = dateEdit->date();
    model->setData(index, date.toString("dd-MM-yyyy"));
}


void ItemDelegateDate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}
