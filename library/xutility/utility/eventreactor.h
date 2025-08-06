#ifndef EVENTREACTOR_H
#define EVENTREACTOR_H

struct IEventReactor
{
	virtual bool execute(QObject* o, QEvent* e) = 0;
	virtual ~IEventReactor() {}
};

template <class Watched, class Event, class Reactor
	, typename = typename std::enable_if<
		std::is_base_of<QObject, Watched>::value
		&& std::is_base_of<QEvent, Event>::value
		&& std::is_class<Reactor>::value
	>::type>
class EventReactor : public IEventReactor
{
	using Work = bool(Reactor::*)(Watched*, Event*);
	Reactor* reactor;
	Work work;
public:
	EventReactor(Reactor* reactor, Work work) : reactor(reactor), work(work) {}

	bool execute(QObject* o, QEvent* e) override
	{
		if (reactor == nullptr || work == nullptr || o == nullptr || e == nullptr) return false;
		return (reactor->*work)(qobject_cast<Watched*>(o), static_cast<Event*>(e));
	}
};

#endif //EVENTREACTOR_H