#ifndef UTILITY_H
#define UTILITY_H

#include <QFont>
#include <QLocale>

#include <QObject>
#if QT_VERSION_MAJOR >= 6
#else //QT_VERSION_MAJOR
#include <memory>
#include <QMargins>
Q_DECLARE_METATYPE(QMargins);
Q_DECLARE_METATYPE(QFont::Style);
Q_DECLARE_METATYPE(QFont::StyleHint);
Q_DECLARE_METATYPE(QFont::SpacingType);
Q_DECLARE_METATYPE(QFont::HintingPreference);
Q_DECLARE_METATYPE(QFont::Capitalization);
Q_DECLARE_METATYPE(Qt::WindowModality);
Q_DECLARE_METATYPE(Qt::FocusPolicy);

#endif //QT_VERSION_MAJOR

namespace tools {

template<class ObjectType>
bool IsType(QObject* o)
{
    return o != nullptr ? o->inherits(ObjectType::staticMetaObject.className()) : false;
}

template <typename Arg>
std::string key(const Arg&& arg)
{
    return std::string(arg);
}

template <typename Arg, typename ...Args>
std::string key(const Arg&& arg, const Args&& ...args)
{
    return std::string(arg) + "-" + key(std::forward<Args>(args)...);
}

//is c belong [0-9a-zA-Z_]
bool IsValueNameChar(const char& c);

//屏幕区域
QRect GetPrimaryScreenAvailableGeometry();

bool Convert(QVariant& variant, int targetType);

QModelIndex SiblingAtColumn(const QModelIndex& index, int column);

QVariant Families(const QFont& font);

QLocale::Country Country(const QLocale& locale);

//从typeName:in中检出QMetaEnum名字，flags:is QFlags<Enum>
std::string CheckOutEnumName(const char *in, bool &flags);

//从QVariant:prop中检出QMetaEnum
QMetaEnum CheckOutMetaEnum(const QVariant& prop, bool& flags);

//从QMetaProperty中检出属性
QVariant CheckOutValue(const QMetaProperty& prop, const QObject* obj);

//修正从QMetaProperty:prop中读出的QMetaEnum型值QVariant:value
void CorrectMetaEnumValue(const QMetaProperty& prop, QVariant& value);

}

#endif // UTILITY_H