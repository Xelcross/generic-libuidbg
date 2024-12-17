#include "propertyitemdelegate.h"

PropertyItemDelegate::PropertyItemDelegate(QObject *parent)
    : Base(parent)
{
    auto factory = itemEditorFactory();
    if (factory == nullptr) {
        setItemEditorFactory(factory = new QItemEditorFactory);
    }
    factory->registerEditor(QMetaType::Bool, new QItemEditorCreator<QCheckBox>("Bool"));
}

// painting
void PropertyItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const auto prop = index.data(Qt::EditRole);
    if (prop.userType() == QMetaType::Bool) {
        QStyleOptionButton button;
        const auto widget = option.widget;
        button.rect = option.rect;
        button.state |= QStyle::State_Enabled | QStyle::State_Editing;
        button.state |= prop.value<bool>()? QStyle::State_On : QStyle::State_Off;

        QStyle *style = widget ? widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_CheckBox, &button, painter, widget);
    } else {
        Base::paint(painter, option, index);
    }
}

QWidget *PropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget* widget = nullptr;
    do {
        const auto prop = index.data(Qt::EditRole);
        bool flags(false);
        const auto em = utility::CheckOutMetaEnum(prop, flags);
        if (!em.isValid() || em.isFlag() || flags) break;

        auto comboBox = new QComboBox(parent);
        for (auto i = 0; i < em.keyCount(); ++i) {
            auto v = QVariant::fromValue(em.value(i));
            utility::Convert(v, prop.userType());
            comboBox->addItem(em.key(i), v);
        }
        widget = comboBox;
    } while (false);

    if (widget == nullptr)
        widget = Base::createEditor(parent, option, index);
    return widget;
}

void PropertyItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (utility::IsType<QComboBox>(editor)) {
        editor->setProperty("currentText", index.data(Qt::EditRole));
    } else {
        Base::setEditorData(editor, index);
    }
}

void PropertyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (utility::IsType<QComboBox>(editor)) {
        auto v = editor->property("currentData");
        model->setData(index, v);
    } else {
        Base::setModelData(editor, model, index);
    }
}

DEFINE_PRODUCTIONLINE_DEFAULT(QAbstractItemDelegate, PropertyItemDelegate, QObject*);
