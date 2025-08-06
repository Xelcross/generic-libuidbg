#ifndef CHECKBOX_STYLEDITEMDELEGATE_H
#define CHECKBOX_STYLEDITEMDELEGATE_H

#include "../xutility_global.h"
#include "lite/pridata.h"

#include <QtWidgets/QStyledItemDelegate>

class XUTILITY_EXPORT CheckBox_StyledItemDelegate : public QStyledItemDelegate
{
	using Base = QStyledItemDelegate;
	PriData
	Q_OBJECT
public:
	explicit CheckBox_StyledItemDelegate(QObject *parent = nullptr);
	~CheckBox_StyledItemDelegate();

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option, const QModelIndex &index) const override;

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option, const QModelIndex &index) override;
};

#endif //CHECKBOX_STYLEDITEMDELEGATE_H