#include "propparser.h"

QList<QStandardItem*> generateItems(const QVariant& name, const QVariant& prop)
{
    auto first = new QStandardItem;
    first->setData(name, Qt::EditRole);
 //   first->setEditable(false);
    auto second = new QStandardItem;
    second->setData(prop, Qt::EditRole);
//    second->setEditable(false);
    return { first, second };
}

template <typename Type>
struct PropParserX : PropParser {
    typedef Type PropType;
    typedef PropParserX Base;

    virtual void generateChildren(QList<QVariantList>& values, const PropType& value, const QVariant& prop) = 0;

    void parse(const QVariant& prop, QList<QStandardItem*>& items) final
    {
        if (!prop.canConvert<PropType>()) return;
        const auto value = prop.value<PropType>();
        QList<QVariantList> values;
        generateChildren(values, value, prop);
        std::unique_ptr<PropParser> parser;
        for (const auto& v : values) {
            auto itemList = generateItems(v.first(), v.last());
            const auto typeName = v.last().typeName();
            if (!parser || parser->typeName() != typeName) {
                parser.reset(Factory<PropParser>::produce(typeName));
            }
            items.first()->appendRow(itemList);
            if (!parser) continue;
            parser->parse(v.last(), itemList);
        }
        items.last()->setEditable(values.empty());
    }
};

struct PropParser_QEnum : PropParserX<int> {
    std::string typeName() const final { return "QEnum"; }

    void generateChildren(QList<QVariantList>& values, const PropType& value, const QVariant& prop) override
    {
        do {
            bool flags(false);
            const auto em = utility::CheckOutMetaEnum(prop, flags);
            if (!em.isValid() || !flags) break;

            for (auto i = 0; i < em.keyCount(); ++i) {
                values.push_back({ em.key(i), (value & em.value(i)) != 0 });
            }
        } while (false);
    }
};
DEFINE_PRODUCTIONLINE(PropParser, PropParser_QEnum, std::string, "QEnum");


#define BEGIN_DEFINE_PROPPARSER(ClassType) \
struct PropParser_##ClassType: PropParserX<ClassType> { \
    std::string typeName() const final { return #ClassType; }

#define END_DEFINE_PROPARSER(ClassType) };    \
DEFINE_PRODUCTIONLINE(PropParser, PropParser_##ClassType, std::string, #ClassType)

BEGIN_DEFINE_PROPPARSER(QRect)
void generateChildren(QList<QVariantList>& values, const PropType& value, const QVariant& prop) override
{
    values.push_back({ "x", value.x() });
    values.push_back({ "y", value.y() });
    values.push_back({ "width", value.width() });
    values.push_back({ "height", value.height() });
}
END_DEFINE_PROPARSER(QRect)


BEGIN_DEFINE_PROPPARSER(QPoint)
void generateChildren(QList<QVariantList>& values, const PropType& value, const QVariant& prop) override
{
    values.push_back({ "x", value.x() });
    values.push_back({ "y", value.y() });
}
END_DEFINE_PROPARSER(QPoint)


BEGIN_DEFINE_PROPPARSER(QSize)
void generateChildren(QList<QVariantList>& values, const PropType& value, const QVariant& prop) override
{
    values.push_back({ "width", value.width() });
    values.push_back({ "height", value.height() });
}
END_DEFINE_PROPARSER(QSize)


BEGIN_DEFINE_PROPPARSER(QSizePolicy)
void generateChildren(QList<QVariantList>& values, const PropType& value, const QVariant& prop) override
{
    values.push_back({ "horizontalPolicy", QVariant::fromValue(value.horizontalPolicy()) });
    values.push_back({ "verticalPolicy", QVariant::fromValue(value.verticalPolicy()) });
    values.push_back({ "horizontalStetch", value.horizontalStretch() });
    values.push_back({ "verticalStretch", value.verticalStretch() });
}
END_DEFINE_PROPARSER(QSizePolicy)


BEGIN_DEFINE_PROPPARSER(QRegion)
void generateChildren(QList<QVariantList>& values, const PropType& value, const QVariant& prop) override
{
}
END_DEFINE_PROPARSER(QRegion)


BEGIN_DEFINE_PROPPARSER(QPalette)
void generateChildren(QList<QVariantList>& values, const PropType& value, const QVariant& prop) override
{
}
END_DEFINE_PROPARSER(QPalette)


BEGIN_DEFINE_PROPPARSER(QFont)
void generateChildren(QList<QVariantList>& values, const PropType& value, const QVariant& prop) override
{
    values.push_back({ "families", utility::Families(value) });
    values.push_back({ "styleName", value.styleName() });
    values.push_back({ "pointSize", value.pointSize() });
    values.push_back({ "pointSizeF", value.pointSizeF() });
    values.push_back({ "pixelSize", value.pixelSize() });
    values.push_back({ "bold", value.bold() });
    values.push_back({ "italic", value.italic() });
    values.push_back({ "underline", value.underline() });
    values.push_back({ "overline", value.overline() });
    values.push_back({ "strikeOut", value.strikeOut() });
    values.push_back({ "fixedPitch", value.fixedPitch() });
    values.push_back({ "kerning", value.kerning() });
    values.push_back({ "stretch", value.stretch() });
    values.push_back({ "letterSpacing", value.letterSpacing() });
    values.push_back({ "wordSpacing", value.wordSpacing() });
    values.push_back({ "exactMatch", value.exactMatch() });
    values.push_back({ "weight",  QVariant::fromValue(value.weight()) });
    values.push_back({ "style",  QVariant::fromValue(value.style()) });
    values.push_back({ "styleHint",  QVariant::fromValue<QFont::StyleHint>(value.styleHint()) });
    values.push_back({ "styleStrategy",  QVariant::fromValue(value.styleStrategy()) });
    values.push_back({ "letterSpacingType",  QVariant::fromValue<QFont::SpacingType>(value.letterSpacingType()) });
    values.push_back({ "hintingPreference",  QVariant::fromValue<QFont::HintingPreference>(value.hintingPreference()) });
    values.push_back({ "capitalization",  QVariant::fromValue<QFont::Capitalization>(value.capitalization()) });
}
END_DEFINE_PROPARSER(QFont)


BEGIN_DEFINE_PROPPARSER(QCursor)
void generateChildren(QList<QVariantList>& values, const PropType& value, const QVariant& prop) override
{
}
END_DEFINE_PROPARSER(QCursor)


BEGIN_DEFINE_PROPPARSER(QIcon)
void generateChildren(QList<QVariantList>& values, const PropType& value, const QVariant& prop) override
{
  values.push_back({ "name", value.name()});
}
END_DEFINE_PROPARSER(QIcon)


BEGIN_DEFINE_PROPPARSER(QLocale)
void generateChildren(QList<QVariantList>& values, const PropType& value, const QVariant& prop) override
{
    values.push_back({ "Language", QVariant::fromValue(value.language()) });
    values.push_back({ "Script", QVariant::fromValue(value.script()) });
    values.push_back({ "Country", QVariant::fromValue(utility::Country(value)) });
}
END_DEFINE_PROPARSER(QLocale)

BEGIN_DEFINE_PROPPARSER(QMargins)
void generateChildren(QList<QVariantList>& values, const PropType& value, const QVariant& prop) override
{
    values.push_back( { "left", value.left() });
    values.push_back( { "right", value.top() });
    values.push_back( { "right", value.right() });
    values.push_back( { "bottom", value.bottom() });
}
END_DEFINE_PROPARSER(QMargins)
