#ifndef ITEMDELEGATECHECKBOX_H
#define ITEMDELEGATECHECKBOX_H

#include <QItemDelegate>

class ItemDelegateCheckBox : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ItemDelegateCheckBox(QObject *parent = 0);

protected:
    QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

};

#endif // ITEMDELEGATECHECKBOX_H
