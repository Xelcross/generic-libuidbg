#ifndef APPWIDGET_H
#define APPWIDGET_H

class AppWidget : public CommandWidget
{
	using Base = CommandWidget;
	PriData
	Q_OBJECT

public:
	explicit AppWidget(QWidget* parent = nullptr);
	~AppWidget();

protected:
	void initializeAction() override;

protected slots:
	void on_SelectorItemClicked(const QModelIndex& index);

	void on_SelectorItemCurrentChanged(const QModelIndex& current
		, const QModelIndex& previous);
};

#endif //APPWIDGET_H