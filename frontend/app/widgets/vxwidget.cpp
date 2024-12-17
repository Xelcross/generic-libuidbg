#include "ui_vxwidget.h"
#include "vxwidget.h"

struct VXWidget::Data {
};

VXWidget::VXWidget(QWidget* parent)
	: Base(parent)
    , d(new VXWidget::Data)
{
	Ui::Form ui;
    ui.setupUi(this);
}

VXWidget::~VXWidget()
{
}

DEFINE_PRODUCTIONLINE_DEFAULT(QWidget, VXWidget)
