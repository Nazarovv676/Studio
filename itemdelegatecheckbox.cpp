#include "itemdelegatecheckbox.h"
#include <QCheckBox>

ItemDelegateCheckBox::ItemDelegateCheckBox(QObject *parent) : QItemDelegate(parent)
{

}

QWidget *ItemDelegateCheckBox::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    QCheckBox *editor = new QCheckBox(parent);
    return editor;
}

void ItemDelegateCheckBox::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    bool value =index.model()->data(index, Qt::EditRole).toBool();
    QCheckBox *checkBox = static_cast<QCheckBox*>(editor);
    checkBox->setChecked(value);
}

void ItemDelegateCheckBox::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QCheckBox *checkBox = static_cast<QCheckBox*>(editor);
    int data = checkBox->isChecked();
    model->setData(index, QString::number(data));
}


void ItemDelegateCheckBox::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}
