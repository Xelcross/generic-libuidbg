#ifndef SPOTWIDGET_H
#define SPOTWIDGET_H

#include "pridata.h"
#include <QWidget>

class SpotWidget : public QWidget
{
    PriData
public:
    explicit SpotWidget(QWidget *parent = nullptr);
    ~SpotWidget();

    void spot(const QRect& geo);

protected:
    void paintEvent(QPaintEvent* e) override;
    void updateIndex();
};

#endif // SPOTWIDGET_H
