#include "eventfilter_shell.h"

struct EventFilter_Shell::Data {
	QList<QEvent::Type> watchEventList;
	std::function<bool(QObject*, QEvent*)> reactor;
};

EventFilter_Shell::EventFilter_Shell(QList<QEvent::Type> watchEventList, std::function<bool(QObject*, QEvent*)>&& reactor, QObject *parent)
	: Base(parent)
	, d(new EventFilter_Shell::Data)
{
	d->watchEventList = watchEventList;
	d->reactor = std::forward<std::function<bool(QObject*, QEvent*)>>(reactor);
}

EventFilter_Shell::~EventFilter_Shell()
{
}

bool EventFilter_Shell::eventFilter(QObject * watched, QEvent * event)
{
	auto filter = false;
	if (d->watchEventList.contains(event->type()) && d->reactor) {
		filter = d->reactor(watched, event);
	}
	return filter || Base::eventFilter(watched, event);
}
