#include "commandwidget.h"
#include "progresswidget.h"
#include "xutility/command/commandcenter.h"
#include "../utility/tools.h"

struct CommandWidget::Data {
	bool initialized;
	bool initialized_delay;
	bool listenresult_selftype_only;

	Data() : initialized(false), initialized_delay(false), listenresult_selftype_only(true) { }
};

CommandWidget::CommandWidget(QWidget *parent)
	: QWidget(parent)
	, d(new CommandWidget::Data)
{
}

CommandWidget::~CommandWidget()
{
}

void CommandWidget::initialize()
{
	if (!d->initialized) {
		d->initialized = true;
		initializeAction();
		CommandCenter::instance()->linkUi(this);
	}
}

bool CommandWidget::isInitialized() const
{
	return d->initialized;
}

bool CommandWidget::isInitializedDelay() const
{
	return d->initialized_delay;
}

void CommandWidget::reset()
{
	if (!d->initialized) {
		initialize();
	} else {
		resetAction();
	}
}

bool CommandWidget::listenResultSelfTypeOnly() const
{
	return d->listenresult_selftype_only;
}

void CommandWidget::setListenResultSelfTypeOnly(bool value)
{
	d->listenresult_selftype_only = value;
}

void CommandWidget::initializeAction()
{
	//implementation
}

void CommandWidget::resetAction()
{
	//implementation
}

void CommandWidget::initializeDelayAction()
{
	//implementation
}

void CommandWidget::initializeDelay()
{
	initializeDelayAction();
}

void CommandWidget::showEvent(QShowEvent * event)
{
	if (!d->initialized_delay) {
		d->initialized_delay = true;
		initializeDelay();
	}
	Base::showEvent(event);
}

void CommandWidget::commandExecute(const QString & name, const QVariant & param)
{
	if (!canCommandExecute(name, param)) return;

	ProgressWidget progressWidget(this);
	progressWidget.show();
	emit sig_CommandExecute(name, param);
}

bool CommandWidget::canCommandExecute(const QString & name, const QVariant & param)
{
	return true;
}

void CommandWidget::onCommandExecuteResult(const QString & name, const QVariant & param, const QVariant & result, const QString& caller)
{
	auto meta = metaObject();
	if (listenResultSelfTypeOnly() && !caller.isEmpty() && caller != meta->className())
		return;

	auto&& method_name = ("onResult_" + name).toStdString();
	if (!xutility::HasMethod(meta, method_name.data()))
		return;

	QMetaObject::invokeMethod(this
		, method_name.data()
		, Q_ARG(const QVariant&, param)
		, Q_ARG(const QVariant&, result)
	);
}
