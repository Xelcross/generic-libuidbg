#ifndef CHECKBOX_HEADERVIEW_H
#define CHECKBOX_HEADERVIEW_H

#include "../xutility_global.h"
#include "lite/pridata.h"

#include <QHeaderView>

class XUTILITY_EXPORT Checkbox_HeaderView : public QHeaderView
{
	using Base = QHeaderView;
	PriData
	Q_OBJECT
	Q_PROPERTY(int checkableLogicalIndex READ checkableLogicalIndex WRITE setCheckableLogicalIndex)

public:
	explicit Checkbox_HeaderView(Qt::Orientation orientation, QWidget *parent = nullptr);
	~Checkbox_HeaderView();

	int checkableLogicalIndex() const;
	void setCheckableLogicalIndex(int index);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;

	Qt::CheckState  checkState() const;
	void setCheckState(Qt::CheckState state);

	void slot_sectionClicked(int logicalIndex);
};

#endif //CHECKBOX_HEADERVIEW_H