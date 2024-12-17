#ifndef PROPERTYITEMDELEGATE_H
#define PROPERTYITEMDELEGATE_H

class PropertyItemDelegate : public QStyledItemDelegate
{
    typedef QStyledItemDelegate Base;
public:
    explicit PropertyItemDelegate(QObject *parent = nullptr);

    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    // editing
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const override;

};

#endif // PROPERTYITEMDELEGATE_H
