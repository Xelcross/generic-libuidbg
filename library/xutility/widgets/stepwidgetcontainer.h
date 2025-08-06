#ifndef STEPWIDGETCONTAINER_H
#define STEPWIDGETCONTAINER_H

#include "../xutility_global.h"
#include "lite/pridata.h"
#include <QWidget>

/*
* @brief    流程界面管理
*	start(widgetName);	根据界面类名称创建QWidget的派生类，parent设置为StepWidgetContainer，并填充到StepWidgetContainer内，resize自适应
*  流程子界面：className@widgetName，根据需要创建信号：
*		1. sig_stepQuit		退出流程，关闭StepWidgetContainer
*		2. sig_nextStep(const QString& widgetName, const QString& method = QString(), const QVariant& param = QVariant())
*									进入下一个流程子界面：widgetName：界面类名称；method：进入下个界面后通过invokeMethod调用的函数; param: method函数需要的参数
*
*	要点：	1. 流程子界面 WidgetClass 需要在cpp中新增一行代码 DEFINE_PRODUCTIONLINE_DEFAULT(QWidget, WidgetClass)，用来给类工厂注册生产线
*				2. 如有跳转下一步流程或退出的需要，请在适当时机发送信号 sig_nextStep(...) 或 sig_stepQuit()
*				3. 如跳转下一步流程后需要执行指定的函数，请在需要执行的函数声明前加 Q_INVOKABLE，或将其设置成槽函数，若有接收参数，类型设置为const QVariant&,
*/
class XUTILITY_EXPORT StepWidgetContainer : public QWidget
{
	using Base = QWidget;
	PriData
	Q_OBJECT

public:
	explicit StepWidgetContainer(QWidget *parent = nullptr);
	~StepWidgetContainer();

	void start(const QString& widgetName);

	void setCurrentWidget(const QString& widgetName);

	QHash<QString, QPointer<QWidget>> widgetMap() const;

protected slots:
	void slot_nextStep(const QString&, const QString&, const QVariant&);
};

#endif //STEPWIDGETCONTAINER_H
