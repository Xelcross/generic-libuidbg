#ifndef DBGGENERICPLUGIN_H
#define DBGGENERICPLUGIN_H

#include "pridata.h"
#include <QGenericPlugin>

class DbgGenericPlugin : public QGenericPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QGenericPluginFactoryInterface_iid FILE "uidbg.json")
    PriData

public:
    explicit DbgGenericPlugin(QObject *parent = nullptr);
    ~DbgGenericPlugin();

    bool event(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QObject *create(const QString &name, const QString &spec) override;
    void showObjectTree();
};

#endif // DBGGENERICPLUGIN_H
