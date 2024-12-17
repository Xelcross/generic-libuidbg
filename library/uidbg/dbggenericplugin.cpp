#include "dbggenericplugin.h"

struct DbgGenericPlugin::Data
{
    DbgGenericPlugin* q;
    QPointer<QWidget> console;
    QPointer<QWidget> objtree;
    QHash<int, void (Data::*)()> keyActions;

    Data() : q(nullptr), console(nullptr), objtree(nullptr) {}

    void init(DbgGenericPlugin* dbg)
    {
        if (q != nullptr) return;
        q = dbg;
        qApp->installEventFilter(q);

        keyActions.insert(Qt::Key_D, &Data::showConsole);
        keyActions.insert(Qt::Key_C, &Data::showObjTree);
        keyActions.insert(Qt::Key_F, &Data::captureWidget);
    }

    bool createWidget(const char* wName, QPointer<QWidget>& w)
    {
        if (w != nullptr) return false;
        w = Factory<QWidget>::produce<QWidget*>(wName, nullptr);
        w->setWindowFlag(Qt::Window);
        w->setAttribute(Qt::WA_DeleteOnClose);
        return true;
    }

    void showConsole()
    {
        if (createWidget("ConsoleWidget", console)) {
            const auto btn = console->findChild<QPushButton*>("ObjTree");
            if (btn != nullptr) {
                q->connect(btn, &QPushButton::clicked, q, &DbgGenericPlugin::showObjectTree);
                auto geometry = utility::GetPrimaryScreenAvailableGeometry();
                geometry.setLeft(geometry.width() * 2 / 3);
                geometry.setTop(geometry.height() * 2 / 3);
                console->setGeometry(geometry);
            }
        }
        console->show();
        console->raise();
    }

    void showObjTree()
    {
        if (createWidget("ObjectTreeWidget", objtree)) {
            auto geometry = utility::GetPrimaryScreenAvailableGeometry();
            geometry.setLeft(geometry.width() * 2 / 3);
            geometry.setTop(qApp->style()->pixelMetric(QStyle::PM_TitleBarHeight));
            geometry.setHeight(geometry.height() * 2 / 3 - geometry.top());
            objtree->setGeometry(geometry);
        }
        objtree->show();
        objtree->raise();
    }

    void captureWidget()
    {
        auto w = qApp->widgetAt(QCursor::pos());
        if (w == nullptr) return;
        showObjTree();
        qLogInfo << w->metaObject()->className();
        objtree->setProperty("capturedObject", QVariant::fromValue(w));
    }
};

DbgGenericPlugin::DbgGenericPlugin(QObject *parent)
    : QGenericPlugin(parent)
    , d(new DbgGenericPlugin::Data)
{
    d->init(this);
}

DbgGenericPlugin::~DbgGenericPlugin()
{
}

bool DbgGenericPlugin::event(QEvent *event)
{
    return QGenericPlugin::event(event);
}

bool DbgGenericPlugin::eventFilter(QObject *watched, QEvent *event)
{
    do {
        const auto type = event->type();
        if (type != QEvent::KeyPress && type != QEvent::KeyRelease) break;
        if (!watched->isWindowType()) break;

        auto ke = static_cast<QKeyEvent*>(event);
        if (ke->isAutoRepeat()) break;
        if (ke->modifiers() != (Qt::AltModifier | Qt::ShiftModifier)) break;

        const auto fi = d->keyActions.find(ke->key());
        if (fi == d->keyActions.end()) break;

        const auto isPress = type == QEvent::KeyPress;
        const auto isKeyF = fi.key() == Qt::Key_F;
        qApp->setOverrideCursor((isKeyF && isPress) ? Qt::CrossCursor : Qt::ArrowCursor);
        if (isPress) {
            if (isKeyF && d->objtree != nullptr) d->objtree->hide();
            break;
        }

        (d.get()->*fi.value())();
        event->accept();
    } while (false);
    return QGenericPlugin::eventFilter(watched, event);
}

QObject *DbgGenericPlugin::create(const QString &name, const QString &spec)
{
    return nullptr;
}

void DbgGenericPlugin::showObjectTree()
{
    d->showObjTree();
}
