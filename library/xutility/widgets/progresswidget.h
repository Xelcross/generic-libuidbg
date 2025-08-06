#pragma once

#include "../xutility_global.h"
#include "lite/pridata.h"
#include <QWidget>

class XUTILITY_EXPORT ProgressWidget : public QWidget
{
	using Base = QWidget;
	PriData
	Q_OBJECT
public:
	explicit ProgressWidget(QWidget *parent = nullptr);
	explicit ProgressWidget(const QString& displayName, QWidget *parent = nullptr);
	~ProgressWidget();

	QString displayName() const;

	void setAnchorAlignment(Qt::Alignment alignment);

public slots:
	void setDisplayName(const QString& name);

protected:
	void paintEvent(QPaintEvent *event) override;
	void timerEvent(QTimerEvent *event) override;
	void showEvent(QShowEvent *event) override;
	void closeEvent(QCloseEvent* e) override;

	void initialize();
	Q_INVOKABLE void setDisplayNameAction(const QString& name);
};
