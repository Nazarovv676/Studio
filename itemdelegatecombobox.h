#ifndef ITEMDELEGATECOMBOBOX_H
#define ITEMDELEGATECOMBOBOX_H

#include <QItemDelegate>

class ItemDelegateComboBox : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ItemDelegateComboBox(QStringList indexList, QStringList shownList, QObject *parent = 0);

protected:
    QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QStringList _indexList;
    QStringList _shownList;

};

#endif // ITEMDELEGATECOMBOBOX_H
