#include "itemdelegatespinbox.h"
#include <QSpinBox>

ItemDelegateSpinBox::ItemDelegateSpinBox(int minValue, int maxValue, QObject *parent) : QItemDelegate(parent), _minValue(minValue), _maxValue(maxValue)
{

}

QWidget *ItemDelegateSpinBox::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    QSpinBox *editor = new QSpinBox(parent);
    editor->setRange(_minValue, _maxValue);
    return editor;
}

void ItemDelegateSpinBox::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    int value =index.model()->data(index, Qt::EditRole).toInt();
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}

void ItemDelegateSpinBox::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    int data = spinBox->value();
    model->setData(index, QString::number(data));
}


void ItemDelegateSpinBox::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}
