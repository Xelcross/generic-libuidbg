#pragma once

#include "../xutility_global.h"
#include "lite/pridata.h"
#include <QEvent>
#include <QObject>
#include <functional>

class XUTILITY_EXPORT EventFilter_Shell : public QObject
{
	using Base = QObject;
	PriData
public:
	EventFilter_Shell(QList<QEvent::Type> watchEventList, std::function<bool(QObject*, QEvent*)>&& reactor, QObject *parent);
	~EventFilter_Shell();

protected:
	bool eventFilter(QObject *watched, QEvent *event) override;
};
