#include "stepwidgetcontainer.h"
#include "lite/factory.h"
#include "xutility/eventfilter/eventfilter_shell.h"
#include "../utility/tools.h"
#include <QtGui/QtGui>

struct StepWidgetContainer::Data {
	QHash<QString, QPointer<QWidget>> widgetHashMap;
	QString currentWidgetName;
	StepWidgetContainer* q;
	QPropertyAnimation animation;

	Data() : q(nullptr) { }

	void showWidget(QWidget* widget, const QRect& geometry)
	{
		if (widget == nullptr) return;
		auto start = geometry;
		start.moveRight(0);

		animation.setTargetObject(widget);
		animation.setProperty("TargetName", QString(widget->metaObject()->className()));
		animation.setPropertyName("geometry");
		animation.setStartValue(start);
		animation.setEndValue(geometry);
		animation.setDirection(QAbstractAnimation::Direction::Forward);
		animation.setDuration(200);
		animation.start();
		widget->show();
	}

	void switchWidget(QWidget* widget, const QRect& geometry)
	{
		if (auto target = widgetHashMap.value(animation.property("TargetName").toString())) {
			animation.setDirection(QAbstractAnimation::Direction::Backward);
			connect(&animation, &QAbstractAnimation::finished, q, [=]() {
				auto r = disconnect(&animation, &QAbstractAnimation::finished, q, nullptr);
				target->close();
				showWidget(widget, geometry);
			}, Qt::QueuedConnection);
			animation.start();
		} else {
			showWidget(widget, geometry);
		}
	}

	void setCurrentWidget(const QString& widgetName)
	{
		if (widgetName.trimmed() == currentWidgetName) return;
		currentWidgetName = widgetName.trimmed();
		auto& current = widgetHashMap[currentWidgetName];
		if (current == nullptr && (current = Factory<QWidget>::produce(currentWidgetName.toStdString()))) {
			current->setParent(q);
			QMetaObject::invokeMethod(current, "initialize");

			if (xutility::HasMethod(current->metaObject(), "sig_nextStep"))
				connect(current, SIGNAL(sig_nextStep(const QString&, const QString&, const QVariant&))
					, q, SLOT(slot_nextStep(const QString&, const QString&, const QVariant&)), Qt::QueuedConnection);

			if (xutility::HasMethod(current->metaObject(), "sig_stepQuit"))
				connect(current, SIGNAL(sig_stepQuit()), q, SLOT(close()), Qt::QueuedConnection);
		}

		switchWidget(current, q->rect());
	}
};

StepWidgetContainer::StepWidgetContainer(QWidget *parent)
	: QWidget(parent)
	, d(new StepWidgetContainer::Data)
{
	d->q = this;
	installEventFilter(new EventFilter_Shell({ QEvent::Resize }, [&](QObject* o, QEvent* e) {
		if (auto current = d->widgetHashMap.value(d->currentWidgetName)) {
			current->setGeometry(rect());
		}
		return false;
	}, this));
	setAttribute(Qt::WA_StyledBackground);
}

StepWidgetContainer::~StepWidgetContainer()
{
}

void StepWidgetContainer::start(const QString & widgetName)
{
	d->setCurrentWidget(widgetName);
	show();
}

void StepWidgetContainer::setCurrentWidget(const QString & widgetName)
{
	d->setCurrentWidget(widgetName);
}

QHash<QString, QPointer<QWidget>> StepWidgetContainer::widgetMap() const
{
	return d->widgetHashMap;
}

void StepWidgetContainer::slot_nextStep(const QString & widgetName, const QString & method, const QVariant & param)
{
	d->setCurrentWidget(widgetName);
	if (auto current = d->widgetHashMap.value(d->currentWidgetName)) {
		const auto methodName = method.trimmed().toStdString();
		if (!methodName.empty()) {
			if (param.isValid()) {
				QMetaObject::invokeMethod(current, methodName.data(), Qt::QueuedConnection, Q_ARG(const QVariant&, param));
			} else {
				QMetaObject::invokeMethod(current, methodName.data(), Qt::QueuedConnection);
			}
		}
	}
}
