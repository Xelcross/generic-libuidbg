#include "checkbox_headerview.h"

#include "lite/factory.h"
#include "lite/qtlog.h"

#include <QPainter>

struct Checkbox_HeaderView::Data {
	QHash<Qt::CheckState, QImage> image_hashmap;
	int checkableLogicalIndex;
	Data() : checkableLogicalIndex(-1) {}
};

Checkbox_HeaderView::Checkbox_HeaderView(Qt::Orientation orientation, QWidget * parent)
	: Base(orientation, parent)
	, d(new Checkbox_HeaderView::Data)
{
	d->image_hashmap[Qt::Checked] = QImage(":/resource/image/checkbox-selected-normal.png");
	d->image_hashmap[Qt::Unchecked] = QImage(":/resource/image/checkbox-unselected-normal.png");
	d->image_hashmap[Qt::PartiallyChecked] = QImage(":/resource/image/checkbox-unselected-normal.png");

	setSectionsClickable(true);
	connect(this, &QHeaderView::sectionClicked, this, &Checkbox_HeaderView::slot_sectionClicked);
}

Checkbox_HeaderView::~Checkbox_HeaderView()
{
}

int Checkbox_HeaderView::checkableLogicalIndex() const
{
	return d->checkableLogicalIndex;
}

void Checkbox_HeaderView::setCheckableLogicalIndex(int index)
{
	d->checkableLogicalIndex = index;
	update();
}

void Checkbox_HeaderView::paintSection(QPainter * painter, const QRect & rect, int logicalIndex) const
{
	if (logicalIndex != d->checkableLogicalIndex) {
		Base::paintSection(painter, rect, logicalIndex);
	} else {
		auto fi = d->image_hashmap.find(checkState());
		if (fi != d->image_hashmap.end()) {
			auto&& image_geo = fi.value().rect();
			image_geo.moveCenter(rect.center());
			painter->drawImage(image_geo, fi.value());
		}
	}
}

Qt::CheckState Checkbox_HeaderView::checkState() const
{
	Qt::CheckState state(Qt::Checked);
	int checked(0);
	if (orientation() == Qt::Horizontal) {
		for (auto r = 0; r < model()->rowCount(); ++r) {
			auto&& check_state = model()->index(r, visualIndex(d->checkableLogicalIndex)).data(Qt::CheckStateRole).value<Qt::CheckState>();
			if (check_state == Qt::Checked)
				++checked;
		}
		if (checked != model()->rowCount()) {
			state = checked == 0 ? Qt::Unchecked : Qt::PartiallyChecked;
		}
	} else if (model()->rowCount() > visualIndex(d->checkableLogicalIndex)) {
		for (auto c = 0; c < model()->columnCount(); ++c) {
			auto&& check_state = model()->index(visualIndex(d->checkableLogicalIndex), c).data(Qt::CheckStateRole).value<Qt::CheckState>();
			if (check_state == Qt::Checked)
				++checked;
		}
		if (checked != model()->columnCount()) {
			state = checked == 0 ? Qt::Unchecked : Qt::PartiallyChecked;
		}
	}
	model()->setHeaderData(d->checkableLogicalIndex, orientation(), state, Qt::CheckStateRole);
	return state;
}

void Checkbox_HeaderView::setCheckState(Qt::CheckState state)
{
	if (orientation() == Qt::Horizontal) {
		for (auto r = 0; r < model()->rowCount(); ++r) {
			auto&& index = model()->index(r, visualIndex(d->checkableLogicalIndex));
			model()->setData(index, state, Qt::CheckStateRole);
		}
	} else if (model()->rowCount() > visualIndex(d->checkableLogicalIndex)) {
		for (auto c = 0; c < model()->columnCount(); ++c) {
			auto&& index = model()->index(visualIndex(d->checkableLogicalIndex), c);
			model()->setData(index, state, Qt::CheckStateRole);
		}
	}
	model()->setHeaderData(d->checkableLogicalIndex, orientation(), state, Qt::CheckStateRole);
}

void Checkbox_HeaderView::slot_sectionClicked(int logicalIndex)
{
	if (logicalIndex != d->checkableLogicalIndex) return;
	auto&& state = model()->headerData(d->checkableLogicalIndex, orientation(), Qt::CheckStateRole).value<Qt::CheckState>();
	state = state == Qt::Checked ? Qt::Unchecked : Qt::Checked;
	setCheckState(state);
}

DEFINE_PRODUCTIONLINE_DEFAULT(QHeaderView, Checkbox_HeaderView, Qt::Orientation)
