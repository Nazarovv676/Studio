#include "itemdelegate.h"

#include <QLineEdit>
#include <QRegularExpressionValidator>

ItemDelegate::ItemDelegate(QRegularExpression *rxvalidator, QObject *parent):QItemDelegate(parent), rxvalidator(rxvalidator)
{

}

QWidget *ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    QLineEdit *editor = new QLineEdit(parent);
    editor->setValidator(new QRegularExpressionValidator(*rxvalidator, parent));
    return editor;
}


void ItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value =index.model()->data(index, Qt::EditRole).toString();
    QLineEdit *line = static_cast<QLineEdit*>(editor);
    line->setText(value);
}


void ItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *line = static_cast<QLineEdit*>(editor);

    QRegularExpressionValidator validator(*rxvalidator, parent());

    int pos = 0;
    QString data = line->text();
    if(validator.validate(data,pos) == QValidator::Acceptable)
        model->setData(index, data);

}


void ItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}
