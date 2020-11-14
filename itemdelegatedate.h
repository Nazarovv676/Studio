#ifndef ITEMDELEGATEDATE_H
#define ITEMDELEGATEDATE_H

#include <QItemDelegate>

class ItemDelegateDate : public QItemDelegate
{
    Q_OBJECT
    QString _dateFormat;
    bool _editable;
public:
    explicit ItemDelegateDate(QString dateFormat, bool editable = false, QObject *parent = 0);

    QString dateFormat() const;

    bool editable() const;

protected:
    QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // ITEMDELEGATEDATE_H
