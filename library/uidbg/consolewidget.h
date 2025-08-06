#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include "pridata.h"
#include <QWidget>

class ConsoleWidget : public QWidget
{
    Q_OBJECT
    PriData
public:
    explicit ConsoleWidget(QWidget *parent = nullptr);
    ~ConsoleWidget();

protected:
    Q_INVOKABLE void writeLog(int type, const QString& log);
};

#endif // CONSOLEWIDGET_H
