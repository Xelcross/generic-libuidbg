#include "appwidget.h"
#include "ui_appwidget.h"

struct AppWidget::Data {
	Ui::AppWidget ui;
	StepWidgetContainer* stepWidgetContainer;

	Data() : stepWidgetContainer(nullptr) { }
};

AppWidget::AppWidget(QWidget * parent)
	: Base(parent)
	, d(new AppWidget::Data)
{
}

AppWidget::~AppWidget()
{
}

void AppWidget::initializeAction()
{
	setAttribute(Qt::WA_StyledBackground);
	d->ui.setupUi(this);
	d->stepWidgetContainer = new StepWidgetContainer(this);
	d->ui.hLayout_workArea->addWidget(d->stepWidgetContainer);
	d->ui.hLayout_workArea->setStretch(1, 1);

	connect(d->ui.selector, &QAbstractItemView::clicked
		, this, &AppWidget::on_SelectorItemClicked);

	auto selection = d->ui.selector->selectionModel();
	if (selection == nullptr) return;
	connect(selection, &QItemSelectionModel::currentChanged
		, this, &AppWidget::on_SelectorItemCurrentChanged);

	auto shortcut = new QShortcut(d->ui.selector);
	connect(shortcut, &QShortcut::activated, d->ui.selector, QOverload<>::of(&QWidget::setFocus));
	shortcut->setKey(QKeySequence(Qt::Key_L + Qt::CTRL + Qt::ALT));
}

void AppWidget::on_SelectorItemClicked(const QModelIndex & index)
{
	if (!index.isValid()) return;
	const auto isExpanded = !d->ui.selector->isExpanded(index);
	d->ui.selector->setExpanded(index, isExpanded);
}

void AppWidget::on_SelectorItemCurrentChanged(const QModelIndex & current
	, const QModelIndex & previous)
{
	if (!current.isValid()) return;
	QVariantMap workWidgetHashMap;
	if (!xutility::GetAppConfigValue(workWidgetHashMap, "WorkWidget")) {
		qLogCritical << "Read Appconfig WorkWidgets Failed!";
		return;
	}

	const auto workWidgetName = workWidgetHashMap.value(
		current.data(Qt::DisplayRole).toString()).toString();
	d->stepWidgetContainer->setCurrentWidget(workWidgetName);
}

DEFINE_PRODUCTIONLINE_DEFAULT(QWidget, AppWidget);
