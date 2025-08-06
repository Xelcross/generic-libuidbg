#include "checkbox_styleditemdelegate.h"
#include "lite/factory.h"

#include <QtWidgets/QtWidgets>

struct CheckBox_StyledItemDelegate::Data {
	QHash<QStyle::State, QImage> image_hashmap;
};

CheckBox_StyledItemDelegate::CheckBox_StyledItemDelegate(QObject * parent)
	: Base(parent)
	, d(new CheckBox_StyledItemDelegate::Data)
{
	d->image_hashmap[QStyle::StateFlag::State_On] = QImage(":/resource/image/checkbox-selected-normal.png");
	d->image_hashmap[QStyle::StateFlag::State_Off] = QImage(":/resource/image/checkbox-unselected-normal.png");
}

CheckBox_StyledItemDelegate::~CheckBox_StyledItemDelegate()
{
}

void CheckBox_StyledItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
	if (opt.state.testFlag(QStyle::State_HasFocus))
		opt.state ^= QStyle::State_HasFocus;
	opt.features &= ~QStyleOptionViewItem::ViewItemFeature::HasCheckIndicator;

	const QWidget *widget = opt.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

	QStyle::State state;
//	if (opt.state.testFlag(QStyle::State_HasFocus))
//		state ^= QStyle::State_HasFocus;
//	if (opt.state.testFlag(QStyle::State_Enabled))
//		state ^= QStyle::State_Enabled;
//	if (opt.state.testFlag(QStyle::State_MouseOver))
//		state ^= QStyle::State_MouseOver;
	const auto value = index.data(Qt::CheckStateRole);
	if (!value.isValid())
		return;

	state |= value.toBool() ? QStyle::State_On : QStyle::State_Off;
	auto fi = d->image_hashmap.find(state);
	if (fi != d->image_hashmap.end()) {
		auto&& image_geo = fi.value().rect();
		image_geo.moveCenter(opt.rect.center());
		painter->drawImage(image_geo, fi.value());
	}
}

bool CheckBox_StyledItemDelegate::editorEvent(QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index)
{
    Q_ASSERT(event);
    Q_ASSERT(model);

    // make sure that the item is checkable
    Qt::ItemFlags flags = model->flags(index);
    if (!(flags & Qt::ItemIsUserCheckable) || !(option.state & QStyle::State_Enabled)
        || !(flags & Qt::ItemIsEnabled))
        return false;

    // make sure that we have a check state
    QVariant value = index.data(Qt::CheckStateRole);
    if (!value.isValid())
        return false;

	const QWidget *widget = option.widget;// QStyledItemDelegatePrivate::widget(option);
    QStyle *style = widget ? widget->style() : QApplication::style();

    // make sure that we have the right event type
    if ((event->type() == QEvent::MouseButtonRelease)
        || (event->type() == QEvent::MouseButtonDblClick)
        || (event->type() == QEvent::MouseButtonPress)) {
        QStyleOptionViewItem viewOpt(option);
        initStyleOption(&viewOpt, index);
		QRect checkRect = option.rect;//style->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &viewOpt, widget);
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        if (me->button() != Qt::LeftButton || !checkRect.contains(me->pos()))
            return false;

        if ((event->type() == QEvent::MouseButtonPress)
            || (event->type() == QEvent::MouseButtonDblClick))
            return true;

    } else if (event->type() == QEvent::KeyPress) {
        if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space
         && static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
            return false;
    } else {
        return false;
    }

    Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
    if (flags & Qt::ItemIsUserTristate)
        state = ((Qt::CheckState)((state + 1) % 3));
    else
        state = (state == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
    return model->setData(index, state, Qt::CheckStateRole);
}

DEFINE_PRODUCTIONLINE_DEFAULT(QAbstractItemDelegate, CheckBox_StyledItemDelegate)
