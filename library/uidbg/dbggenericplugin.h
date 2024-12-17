#ifndef DBGGENERICPLUGIN_H
#define DBGGENERICPLUGIN_H

#include <QGenericPlugin>

class DbgGenericPlugin : public QGenericPlugin
{
    PriData
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QGenericPluginFactoryInterface_iid FILE "uidbg.json")

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
