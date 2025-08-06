#pragma once

#include "../xutility_global.h"
#include <QObject>
#include <functional>

/*
template <class Func, class ...Args>
class ShellObject : public QObject
{
public:
	ShellObject(Func&& func, QObject *parent = nullptr)
		: QObject(parent)
		, m_func(std::forward<Func>(func))
	{
	}

	~ShellObject() { }

public slots:
	Q_INVOKABLE void execute(Args&& ...args)
	{
		if (m_func) {
			m_func(std::forward<Arg>(args)...);
		}
	}

private:
	Func m_func;
};
*/
class XUTILITY_EXPORT ShellObject : public QObject
{
	Q_OBJECT
public:
	explicit ShellObject(std::function<bool(const QVariant&)>&& function, QObject *parent = nullptr);
	~ShellObject() { }

public slots:
	bool execute(const QVariant&);

private:
	std::function<bool(const QVariant&)> m_function;
};
