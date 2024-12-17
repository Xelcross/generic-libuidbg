#pragma once

class VXWidget : public QWidget
{
	using Base = QWidget;
	PriData
	Q_OBJECT
public:
	explicit VXWidget(QWidget* = nullptr);
	~VXWidget();
};
