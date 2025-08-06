#include "shellobject.h"

ShellObject::ShellObject(std::function<bool(const QVariant&)>&& function, QObject *parent)
	: QObject(parent)
	, m_function(function)
{
}

bool ShellObject::execute(const QVariant & arg)
{
	return m_function && m_function(arg);
}
