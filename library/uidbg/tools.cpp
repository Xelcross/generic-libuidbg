#include "qtlog.h"
#include <QApplication>
#include <QModelIndex>
#include <QRect>
#include <QVariant>
#include <QMetaEnum>
#include <QJsonDocument>

#if QT_VERSION_MAJOR >= 6
#include <QScreen>
#else //QT_VERSION_MAJOR
#include <QDesktopWidget>
#endif // QT_VERSION_MAJOR

#include "tools.h"

namespace tools
{

bool IsValueNameChar(const char& c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

QRect GetPrimaryScreenAvailableGeometry()
{
#if QT_VERSION_MAJOR >= 6
    return qApp->primaryScreen()->availableGeometry();
#else //QT_VERSION_MAJOR
    return qApp->desktop()->screenGeometry();
#endif //QT_VERSION_MAJOR
}

bool Convert(QVariant &variant, int targetType)
{
#if QT_VERSION_MAJOR >= 6
    return variant.convert(QMetaType(targetType));
#else //QT_VERSION_MAJOR
    return variant.convert(targetType);
#endif //QT_VERSION_MAJOR
}

QModelIndex SiblingAtColumn(const QModelIndex& index, int column)
{
#if QT_VERSION_MAJOR >= 6
    return index.siblingAtColumn(0);
#else //QT_VERSION_MAJOR
    return index.sibling(index.row(), 0);
#endif //QT_VERSION_MAJOR
}

QVariant Families(const QFont &font)
{
#if QT_VERSION_MAJOR >= 6
    return font.families();
#else //QT_VERSION_MAJOR
    return font.family();
#endif //QT_VERSION_MAJOR
}

QLocale::Country Country(const QLocale &locale)
{
#if QT_VERSION_MAJOR >= 6
    return locale.territory();
#else //QT_VERSION_MAJOR
    return locale.country();
#endif //QT_VERSION_MAJOR
}

std::string CheckOutEnumName(const char *in, bool &flags)
{
    std::string r;
    do {
        auto name = in;
        while (*name != 0 && *name != '<' && *name != ':') ++name;
        if (*name == '<') flags = true;
        while (*name != 0 && *name != ':') ++name;
        while (*name != 0 && *name == ':') ++name;
        int size = 0;
        while (IsValueNameChar(*(name + size))) ++size;
        if (size < 1) break;
        r.append(name, size);
        qLogInfo << "in:" << in << ", name:" << r.c_str() << ", flags:" << flags;
    } while (false);
    return r;
}

QMetaEnum CheckOutMetaEnum(const QVariant &prop, bool &flags)
{
    QMetaEnum r;
    do {
        const QMetaType metaType(prop.userType());
        if (!(metaType.flags() & QMetaType::IsEnumeration)) break;
        const auto mo = metaType.metaObject();
        if (mo == nullptr) break;

        const auto name = CheckOutEnumName(prop.typeName(), flags);
        if (name.empty()) break;
        const auto ioe = mo->indexOfEnumerator(name.c_str());
        if (ioe == -1) break;

        r = mo->enumerator(ioe);
    } while (false);
    return r;
}

QVariant CheckOutValue(const QMetaProperty &prop, const QObject *obj)
{
    QVariant value = prop.read(obj);
    if (prop.isEnumType()) {
//        CorrectMetaEnumValue(prop, value);
    }
    return value;
}

void CorrectMetaEnumValue(const QMetaProperty &prop, QVariant &value)
{
    do {
        const QMetaEnum em = prop.enumerator();
        em.enclosingMetaObject();
        const auto ename = std::string(em.scope()).append("::").append(em.name());
        const auto vname = value.typeName();
        if (ename == vname) break;

        int typeId = QMetaType::type(ename.c_str());
        if (typeId == QMetaType::UnknownType) break;
        if (typeId == value.userType()) break;

        QVariantHash data;
        data["from"] = value;
        data["prop-name"] = prop.typeName();
        data["value-name"] = value.typeName();
        Convert(value, typeId);
        data["to"] = value;
        qLogInfo << QJsonDocument::fromVariant(data).toJson(QJsonDocument::Compact);
    } while (false);
}

}
