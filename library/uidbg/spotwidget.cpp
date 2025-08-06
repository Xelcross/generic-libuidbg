#include <QPainter>
#include <QTimer>
#include "spotwidget.h"

struct SpotWidget::Data
{
    SpotWidget* q;
    QTimer* timer;
    int index;
    int frames;
    Data() : q(nullptr), timer(nullptr), index(-1), frames(6)
    {
    }

    void init(SpotWidget* sw)
    {
        if (q != nullptr) return;
        q = sw;
        q->setWindowFlag(Qt::Window);
        q->setWindowFlag(Qt::FramelessWindowHint);
        q->setWindowFlag(Qt::WindowStaysOnTopHint);
        q->setAttribute(Qt::WA_TranslucentBackground);
        q->setAttribute(Qt::WA_TransparentForMouseEvents);
        timer = new QTimer(q);
        timer->setInterval(500);
        QObject::connect(timer, &QTimer::timeout, q, &SpotWidget::updateIndex);
    }

    void updateIndex()
    {
        ++index;
        if (index > frames) {
            reset();
        } else {
            q->update();
        }
    }

    void reset()
    {
        timer->stop();
        index = -1;
        q->hide();
    }
};

SpotWidget::SpotWidget(QWidget *parent)
    : QWidget(parent)
    , d(new SpotWidget::Data)
{
    d->init(this);
}

SpotWidget::~SpotWidget()
{
}

void SpotWidget::spot(const QRect& geo)
{
    d->reset();
    if (!geo.isValid()) return;
    setGeometry(geo.adjusted(-2, -2, 2, 2));
    show();
    d->timer->start();
}

void SpotWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    if (d->index < 0 || d->index % 2 != 0) return;
    const auto r = rect().adjusted(2, 2, -2, -2);
    QPainter painter(this);
    painter.setPen(QPen(Qt::red, 2));
    painter.drawRect(r);
}

void SpotWidget::updateIndex()
{
    d->updateIndex();
}
