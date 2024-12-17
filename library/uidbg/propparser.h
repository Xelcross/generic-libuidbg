#ifndef PROPPARSER_H
#define PROPPARSER_H

struct PropParser {
    virtual void parse(const QVariant& prop, QList<QStandardItem*>& items) = 0;
    virtual std::string typeName() const = 0;
    virtual ~PropParser() {}
};

QList<QStandardItem*> generateItems(const QVariant& name, const QVariant& prop);

#endif // PROPPARSER_H
