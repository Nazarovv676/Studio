#ifndef ITEMDELEGATESPINBOX_H
#define ITEMDELEGATESPINBOX_H

#include <QItemDelegate>

class ItemDelegateSpinBox : public QItemDelegate
{
    Q_OBJECT
    int _minValue;
    int _maxValue;
public:
    explicit ItemDelegateSpinBox(int minValue, int maxValue, QObject *parent = 0);

protected:
    QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // ITEMDELEGATESPINBOX_H
