#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include "../utility/tools.h"
#include "progresswidget.h"

struct ProgressWidget::Data {
	QString displayName;
	int timerId;
	float angle;
	Qt::Alignment alignment;

	Data() : timerId(0), angle(0.f), alignment(Qt::AlignCenter) { }
};

ProgressWidget::ProgressWidget(QWidget *parent)
	: Base(parent)
	, d(new ProgressWidget::Data)
{
	initialize();
}

ProgressWidget::ProgressWidget(const QString& displayName, QWidget *parent)
	: Base(parent)
	, d(new ProgressWidget::Data)
{
	d->displayName = displayName;
	initialize();
}

ProgressWidget::~ProgressWidget()
{
	if (d->timerId != 0) {
		killTimer(d->timerId);
	}
}

QString ProgressWidget::displayName() const
{
	return d->displayName;
}

void ProgressWidget::setAnchorAlignment(Qt::Alignment alignment)
{
	d->alignment = alignment;
}

void ProgressWidget::setDisplayName(const QString & name)
{
	if (QThread::currentThread() != thread()) {
		QMetaObject::invokeMethod(this, "setDisplayNameAction"
			, Qt::QueuedConnection, Q_ARG(const QString&, name));
	} else {
		setDisplayNameAction(name);
	}
}

void ProgressWidget::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::RenderHint::Antialiasing);
	const auto x = event->rect().height() / 2;
	const auto radius = std::min<>(10.0, x / 5.0);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::darkGray);
	painter.drawRoundedRect(event->rect(), radius, radius);
	painter.save();

	painter.translate(x, x);
	const auto size = 15;
	for (auto i = 0; i < size; ++i) {
		painter.save();
		painter.rotate(d->angle - i * 20.f);
		painter.setPen(Qt::NoPen);
		const auto cv = i * (0xFF - 0x20) / size;
		painter.setBrush(QColor(cv, cv, cv));
		painter.drawEllipse(x * 2.0 / 3.0 -2, -2, 4, 4);
		painter.restore();
	}
	painter.restore();
	painter.setPen(QPen(Qt::green, 10));
	painter.translate(2 * x + 10, x);
	painter.drawText(0, (height() - fontMetrics().height() - 4) / 2, d->displayName);
}
void ProgressWidget::timerEvent(QTimerEvent * event)
{
	if (event->timerId() == d->timerId) {
		d->angle += 5.f;
		update();
	} else {
		Base::timerEvent(event);
	}
}
void ProgressWidget::showEvent(QShowEvent * event)
{
	Base::showEvent(event);
	xutility::AnchorOnParentWidget(this, d->alignment);
}
void ProgressWidget::closeEvent(QCloseEvent * e)
{
	Base::closeEvent(e);
}

void ProgressWidget::initialize()
{
	setWindowFlag(Qt::WindowType::FramelessWindowHint);
	setAttribute(Qt::WidgetAttribute::WA_ShowModal);
	setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
//			setAttribute(Qt::WidgetAttribute::WA_AlwaysStackOnTop);
	d->timerId = startTimer(30);
	setFixedHeight(80);
	setMinimumWidth(height());

	setDisplayName(d->displayName);
}

void ProgressWidget::setDisplayNameAction(const QString & name)
{
	auto f = font();
	if (f.pointSize() < 30) {
		f.setPointSize(30);
		setFont(f);
	}

	d->displayName = name;
	const auto br = fontMetrics().boundingRect(name);
	setFixedWidth(height() + br.width() + 20);
	if (d->timerId != 0) {
		xutility::AnchorOnParentWidget(this, d->alignment);
		update();
	}
}
