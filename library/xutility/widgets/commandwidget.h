#ifndef XUTILITY_COMMANDWIDGET_H
#define XUTILITY_COMMANDWIDGET_H

#include "../xutility_global.h"
#include "lite/pridata.h"

#include <QWidget>
#include <QVariant>

class XUTILITY_EXPORT CommandWidget : public QWidget
{
	using Base = QWidget;
	PriData
	Q_OBJECT
	Q_PROPERTY(bool ListenResultSelfTypeOnly WRITE setListenResultSelfTypeOnly READ listenResultSelfTypeOnly)

public:
	CommandWidget(QWidget *parent = Q_NULLPTR);
	~CommandWidget();

	Q_INVOKABLE void initialize();
	bool isInitialized() const;
	bool isInitializedDelay() const;

	Q_INVOKABLE void reset();

	bool listenResultSelfTypeOnly() const;
	void setListenResultSelfTypeOnly(bool value);

protected:
	virtual void initializeAction();
	virtual void resetAction();
	virtual void initializeDelayAction();

	Q_INVOKABLE void initializeDelay();
	void showEvent(QShowEvent* event) override;

	//通过关联的command执行指令${name}(${param})
	void commandExecute(const QString& name, const QVariant& param = QVariant());
	virtual bool canCommandExecute(const QString& name, const QVariant& param);

protected slots:
	//commandExecute执行结果${result}
	virtual void onCommandExecuteResult(const QString& name, const QVariant& param, const QVariant& result, const QString& caller = QString());

signals:
	void sig_CommandExecute(const QString& name, const QVariant& param);
};

#endif //XUTILITY_COMMANDWIDGET_H