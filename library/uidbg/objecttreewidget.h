#ifndef OBJECTTREEWIDGET_H
#define OBJECTTREEWIDGET_H

class ObjectTreeWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QObject* capturedObject READ capturedObject WRITE setCapturedObject)
    PriData
public:
    explicit ObjectTreeWidget(QWidget *parent = nullptr);
    ~ObjectTreeWidget();

    void setCapturedObject(QObject* obj);
    QObject* capturedObject() const;

protected:
    void loadIndex(const QModelIndex& index);
};

#endif // OBJECTTREEWIDGET_H
