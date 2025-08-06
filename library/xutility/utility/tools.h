#ifndef XUTILITY_TOOLS_H
#define XUTILITY_TOOLS_H

#include "../xutility_global.h"
#include  <QVariant>
#include "lite/tools.h"

namespace xutility
{
////////////////////////////////////////App初始化//////////////////////////////////////////
	XFUNCTION_EXPORT QString GetAppName();

	XFUNCTION_EXPORT QString GetAppComponentPath(const QString& component = {});

	XFUNCTION_EXPORT QString GetAppLogPath();

	XFUNCTION_EXPORT QString GetAppConfigPath();

	XFUNCTION_EXPORT QString GetAppResourcePath();

	XFUNCTION_EXPORT QString GetAppTranslationsPath();

	XFUNCTION_EXPORT void QtMessageHandler_AsLocalFile(QtMsgType, const QMessageLogContext &, const QString &);

	XFUNCTION_EXPORT bool LoadAppConfig();

	XFUNCTION_EXPORT bool LoadAppResource();

	XFUNCTION_EXPORT bool LoadAppCommonQSS();

	XFUNCTION_EXPORT bool LoadAppTranslator();

	//初始化： 加载资源（*.rcc), 加载样式表(app.qss), 加载配置(app.json),加载翻译(app-zh_cn.qm)
	XFUNCTION_EXPORT bool InitializeApplication();

	XFUNCTION_EXPORT QWidget* ShowAppWindow();

	XFUNCTION_EXPORT QVariantMap GetAppConfig();
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////从json型数据中读取指定数据////////////////////////////////////
	//从app.json中取值， ${name}: 取值路径，例如：A[2].B[3][2]
	template <class T>
	bool GetAppConfigValue(T& result, const QString& name)
	{
		return lite::GetJsonValue(result, name, GetAppConfig());
	}
//////////////////////////////////////////////////////////////////////////////////////////

	//设置widget在其parentWidget中位置, anchor on %{screen}，if widget->parentWidget is Null
	XFUNCTION_EXPORT void AnchorOnParentWidget(QWidget* widget, Qt::Alignment alignment = Qt::AlignCenter, int screen = -1);

	//检测元对象${meta}是否存在方法${name}, 定义为Q_SIGNAL, Q_SLOT, Q_INVOKABLE的方法
	XFUNCTION_EXPORT bool HasMethod(const QMetaObject* meta, const char* name);
/*
	//读取电子表格数据 xlsx/xls/csv
	XFUNCTION_EXPORT QList<QVariantList> ReadExcel(const QString& filePath, int sheetIndex);

	XFUNCTION_EXPORT QList<QList<QVariantList>> ReadExcel(const QString& filePath);

	XFUNCTION_EXPORT bool WriteExcel(const QString& filePath, const QStringList& sheetNameList, const QList<QList<QVariantList>>& dataList);
*/
	//资源管理器打开文件
	XFUNCTION_EXPORT void OpenExplorerFile(const QString& file);


	XFUNCTION_EXPORT void SetItemData(QAbstractItemModel* m, int row, int column, const QVariant& display
		, const QMap<int, QVariant>& extral_default = {}, const QModelIndex& parent = QModelIndex());
}

#endif //XUTILITY_TOOLS_H