#include "itemdelegatecombobox.h"

#include "QComboBox"

ItemDelegateComboBox::ItemDelegateComboBox(QStringList indexList, QStringList shownList, QObject *parent):QItemDelegate(parent), _indexList(indexList), _shownList(shownList)
{

}

QWidget *ItemDelegateComboBox::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    QComboBox *editor = new QComboBox(parent);
    int i(0);
    for(QString itemName : _shownList)
        editor->addItem(itemName, _indexList[i++]);
    return editor;
}

void ItemDelegateComboBox::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value =index.model()->data(index, Qt::EditRole).toString();
    QComboBox *combobox = static_cast<QComboBox*>(editor);
    combobox->setCurrentIndex(_indexList.indexOf(value));
}

void ItemDelegateComboBox::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *combobox = static_cast<QComboBox*>(editor);
    QVariant data = combobox->currentData();
    model->setData(index, data);
}


void ItemDelegateComboBox::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}
