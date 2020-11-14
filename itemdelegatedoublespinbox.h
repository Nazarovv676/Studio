#ifndef ITEMDELEGATEDOUBLESPINBOX_H
#define ITEMDELEGATEDOUBLESPINBOX_H

#include <QObject>
#include <QItemDelegate>

class ItemDelegateDoubleSpinBox : public QItemDelegate
{
    Q_OBJECT
    double _minValue;
    double _maxValue;
public:
    explicit ItemDelegateDoubleSpinBox(double minValue, double maxValue, QObject *parent = 0);

protected:
    QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // ITEMDELEGATEDOUBLESPINBOX_H
