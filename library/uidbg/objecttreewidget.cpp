#include <QHeaderView>
#include <QLineEdit>
#include <QMetaProperty>
#include <QPointer>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTreeView>
#include <QVBoxLayout>
#include "factory.h"
#include "propparser.h"
#include "qtlog.h"
#include "spotwidget.h"
#include "tools.h"

#include "objecttreewidget.h"

struct ObjectTreeWidget::Data
{
    ObjectTreeWidget* q;
    QTreeView* objTree;
    QTreeView* propTree;
    QStandardItemModel* objModel;
    QStandardItemModel* propModel;
    QLineEdit* lineEdit;
    QPointer<QObject> capturedObj;
    SpotWidget* spotWidget;
    std::unordered_map<std::string, std::shared_ptr<PropParser>> generators;

    Data()
        : q(nullptr)
        , objTree(nullptr)
        , propTree(nullptr)
        , objModel(nullptr)
        , propModel(nullptr)
        , lineEdit(nullptr)
        , capturedObj(nullptr)
        , spotWidget(nullptr)
    {

    }

    void init(ObjectTreeWidget* ot)
    {
        if (q != nullptr) return;
        q = ot;
		q->setStyleSheet("*{font-family: \"Microsoft YaHei\";font-size: 12pt; background-color: lightGray; color: black; outline: none;}");
        q->resize(600, 800);
        auto vlayout = new QVBoxLayout(q);
        {
            auto splitter = new QSplitter(Qt::Orientation::Horizontal, q);
            splitter->addWidget(objTree = new QTreeView(q));
 //           objTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
            objTree->setModel(objModel = new QStandardItemModel(q));
            QObject::connect(objTree, &QTreeView::clicked, q, &ObjectTreeWidget::loadIndex);

            splitter->addWidget(propTree = new QTreeView(q));
//            propTree->setEditTriggers(QAbstractItemView::AllEditTriggers);
            propTree->setModel(propModel = new QStandardItemModel(q));
            propTree->setItemDelegateForColumn(1,
               Factory<QAbstractItemDelegate>::produce("PropertyItemDelegate", static_cast<QObject*>(propTree)));

            vlayout->addWidget(splitter);
        }
        vlayout->addWidget(lineEdit = new QLineEdit(q));
        lineEdit->setObjectName("CommandLineEdit");
//        lineEdit->hide();
        connect(lineEdit, &QLineEdit::returnPressed, [&](){
			bool ok = false;
            const auto obj = reinterpret_cast<QObject*>(lineEdit->text().trimmed().toLongLong(&ok, 16));
			if (ok && obj != nullptr) {
				setCapturedObject(obj);
			}
/*
            auto fIndex = info.indexOf('.');
            auto objInfo = info.mid(0, fIndex);
            auto propInfo = info.mid(objInfo.size() + 1, info.size() - objInfo.size() - 1);
            objInfo = objInfo.trimmed();
            propInfo = propInfo.trimmed();

            fIndex = objInfo.indexOf('@');
            auto objName = objInfo.mid(0, fIndex);
            auto objClass = objInfo.mid(objName.size() + 1, objInfo.size() - objName.size() - 1);
            objName = objName.trimmed();
            objClass = objClass.trimmed();

            fIndex = propInfo.indexOf('=');
            auto propKey = propInfo.mid(0, fIndex);
            auto propValue = propInfo.mid(propKey.size() + 1, propInfo.size() - propKey.size() - 1);
            propKey = propKey.trimmed();
            propValue = propValue.trimmed();

            bool ok(false);
            auto className = objName;
            const auto objAddress = reinterpret_cast<QObject*>(className.toLongLong(&ok, 16));
            if (ok) {
                className.clear();
            }
            const auto objList = q->findChildren<QObject*>(className);
            QObject* obj(nullptr);
            if (ok && objList.contains(objAddress)) {
                obj = objAddress;
            } else if (!objList.empty()) {
                obj = objList.front();
            }
            if (obj != nullptr) {
                obj->setProperty(propKey.toStdString().data(), QVariant::fromValue(propValue));
            }
            qLogInfo << objName << objClass << propKey << propValue;
*/
        });
    }

    void setCapturedObject(QObject* obj)
    {
        loadObjectTree(obj);
        loadObjectProperty(obj);
        resizeHeaderView();
        capturedObj = obj;
    }

    void resizeHeaderView()
    {
        propTree->expandAll();
        objTree->setHorizontalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
        objTree->setHorizontalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
        propTree->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
        propTree->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
        objTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
        propTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    }

    void setSpotObj(QObject* obj)
    {
        if (spotWidget == nullptr) {
            spotWidget = new SpotWidget(q);
        }

        QRect geo;
        if (obj->isWidgetType()) {
            auto widget = qobject_cast<QWidget*>(obj);
            geo = widget->rect();
            geo.moveTo(widget->mapToGlobal(geo.topLeft()));
        }
        spotWidget->spot(geo);
    }

    QString objName(QObject* obj)
    {
        QString name(obj->objectName());
        if (name.isEmpty())
            name = QString::number((long long)obj, 16).prepend("0x");
        return name;
    }

    QList<QStandardItem*> createObjItem(QObject* obj)
    {
        auto name = new QStandardItem(objName(obj));
        name->setData(QVariant::fromValue(obj));
        auto value = new QStandardItem(obj->metaObject()->className());
        return { name, value };
    }

    QStandardItem* loadChildren(QStandardItem* item, const QObject* spotObj = nullptr)
    {
        QStandardItem* spot(nullptr);
        do {
            if (item->data(Qt::UserRole + 2).toBool()) break;
            auto obj = item->data(Qt::UserRole + 1).value<QObject*>();
            if (obj == nullptr) break;

            for (const auto child : obj->children()) {
                auto items = createObjItem(child);
                item->appendRow(items);
                if (spotObj == child) spot = items[0];
            }
            item->setData(true, Qt::UserRole + 2);
        } while (false);
        return spot;
    }

    void loadIndexChildren(const QModelIndex& index)
    {
        auto findex = tools::SiblingAtColumn(index, 0);
        QObject* obj = capturedObj.isNull() ? nullptr : findex.data(Qt::UserRole + 1).value<QObject*>();
        if (obj != nullptr) {
            loadChildren(objModel->itemFromIndex(findex));
            objTree->expand(findex);

            loadObjectProperty(obj);
            propTree->expandAll();
        } else {
            objModel->clear();
            propModel->clear();
        }
    }


    void loadObjectTree(QObject* obj)
    {
        if (capturedObj == obj) return;

        objModel->clear();
        objModel->setHorizontalHeaderLabels({"Name", "Value"});

        QList<QObject*> objs;
        QObject* rootObj(obj);
        while (rootObj != nullptr) {
            objs.push_front(rootObj);
            rootObj = rootObj->parent();
        }

        QStandardItem* item(nullptr);
        while (!objs.empty()) {
            rootObj = objs.takeFirst();
            if (item == nullptr) {
                auto items = createObjItem(rootObj);
                objModel->appendRow(items);
                item = items[0];
            }
            if (!objs.empty())
                item = loadChildren(item, objs.first());
        }
        if (item != nullptr)
            objTree->setCurrentIndex(item->index());
    }

    void loadObjectProperty(QObject* obj)
    {
        do {
            if (capturedObj == obj) break;
            propModel->clear();
            propModel->setHorizontalHeaderLabels({"Name", "Value"});
            loadDynamicProperty(obj);
            loadMetaData(obj);
        } while (false);
        setSpotObj(obj);
    }

    void loadDynamicProperty(QObject *obj)
    {
        auto item = new QStandardItem("Dynamic properties");
        item->setEditable(false);
        for (const auto &propName : obj->dynamicPropertyNames()) {
            item->appendRow(createPropItem(propName, obj->property(propName)));
        }
        propModel->appendRow(item);
    }

    void loadMetaData(QObject *obj)
    {
        auto mo = obj->metaObject();
        while (mo != nullptr) {
            auto item = new QStandardItem(mo->className());
            item->setEditable(false);
            loadMetaProperty(item, obj, mo);
            loadMetaMethod(item, obj, mo);
            propModel->appendRow(item);
            mo = mo->superClass();
        }
    }

    void loadMetaProperty(QStandardItem* item, QObject* obj, const QMetaObject* mo)
    {
        auto property_item = new QStandardItem("Property :");
        property_item->setEditable(false);
        for (auto i = mo->propertyOffset(); i < mo->propertyCount(); ++i) {
            const auto prop = mo->property(i);
            const auto value = tools::CheckOutValue(prop, obj);
            auto items = createPropItem(prop.name(), value);
            property_item->appendRow(items);
        }
        item->appendRow(property_item);
    }

    void loadMetaMethod(QStandardItem* item, QObject* obj, const QMetaObject* mo)
    {
        static QString names[] = { "Method :", "Signal :", "Slot :", "Construct :" };
        QList<QStandardItem*> rows;
        for (const auto& name : names) {
            auto method = new QStandardItem(name);
            rows.push_back(method);
        }

        for (auto i = mo->methodOffset(); i < mo->methodCount(); ++i) {
            const auto method = mo->method(i);
            QByteArray paramters;
            for (const auto &paramter : method.parameterTypes()) {
                if (paramters.size() != 0)
                    paramters.push_back(", ");
                paramters.push_back(paramter);
            }
            rows[method.methodType()]->appendRow(createPropItem(method.name(), paramters));
        }
        item->appendRows(rows);
    }

    QList<QStandardItem*> createPropItem(const QString& name, const QVariant& prop)
    {
        auto items = generateItems(name, prop);
        do {
            std::shared_ptr<PropParser> generator;
            auto id = prop.typeName();
            if (QMetaType(prop.userType()).flags() & QMetaType::IsEnumeration) {
                id = "QEnum";
            }
            if (id == nullptr) break;
            const auto fi = generators.find(id);
            if (fi != generators.end()) {
                generator = fi->second;
            } else {
                generator.reset(Factory<PropParser>::produce(id));
                generators.insert({ id, generator });
            }
            if (!generator) break;
            generator->parse(prop, items);
        } while (false);
        return items;
    }
};

ObjectTreeWidget::ObjectTreeWidget(QWidget *parent)
    : QWidget(parent)
    , d(new ObjectTreeWidget::Data)
{
    d->init(this);
}

ObjectTreeWidget::~ObjectTreeWidget()
{

}

void ObjectTreeWidget::setCapturedObject(QObject *obj)
{
    d->setCapturedObject(obj);
}

QObject *ObjectTreeWidget::capturedObject() const
{
    return d->capturedObj.data();
}

void ObjectTreeWidget::loadIndex(const QModelIndex &index)
{
    d->loadIndexChildren(index);
}

DEFINE_PRODUCTIONLINE_DEFAULT(QWidget, ObjectTreeWidget, QWidget*)
