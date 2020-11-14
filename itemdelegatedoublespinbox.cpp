#include "itemdelegatedoublespinbox.h"
#include <QDoubleSpinBox>

ItemDelegateDoubleSpinBox::ItemDelegateDoubleSpinBox(double minValue, double maxValue, QObject *parent) : QItemDelegate(parent), _minValue(minValue), _maxValue(maxValue)
{

}

QWidget *ItemDelegateDoubleSpinBox::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setRange(_minValue, _maxValue);
    return editor;
}

void ItemDelegateDoubleSpinBox::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    double value =index.model()->data(index, Qt::EditRole).toDouble();
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(value);
}

void ItemDelegateDoubleSpinBox::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    double data = spinBox->value();
    model->setData(index, QString::number(data));
}


void ItemDelegateDoubleSpinBox::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}
