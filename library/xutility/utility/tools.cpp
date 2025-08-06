#include <QtWidgets/QtWidgets>
#include <memory>
#include <mutex>

#include "lite/factory.h"
#include "lite/qtlog.h"

#include "../utility.h"
#include "../objects/shellobject.h"

namespace xutility
{
	QString GetAppName()
	{
		auto appName = QCoreApplication::applicationName();
#if (defined _DEBUG) || (defined DEBUG)
		if (!appName.isEmpty() && *appName.rbegin() == 'd') appName.chop(1);
#endif
		return appName;
	}

	QString GetAppComponentPath(const QString & component)
	{
		auto&& result = QCoreApplication::applicationDirPath();
		const auto trimmed_component = component.trimmed().toLower();
		if (!trimmed_component.isEmpty()) {
			result.push_back("/" + trimmed_component + "/" + GetAppName());
		}
		return result;
	}

	QString GetAppLogPath()
	{
		return GetAppComponentPath("log");
	}

	QString GetAppConfigPath()
	{
		return GetAppComponentPath("config");
	}

	QString GetAppResourcePath()
	{
		return GetAppComponentPath("mui");
	}

	QString GetAppTranslationsPath()
	{
		return GetAppComponentPath("translations");
	}

	bool SaveLog(const QVariant& msg)
	{
		static QFile file;
		if (file.fileName().isEmpty()) {
			const auto path = GetAppLogPath();
			QDir dir(path);
			if (!dir.exists()) {
				dir.mkpath(path);
			}
			file.setFileName(path + "/"
				+ QDateTime::currentDateTime().toString("yyMMdd-hhmmss") + "_1.txt");
		}
		if (!file.isOpen() && !file.open(QIODevice::WriteOnly | QIODevice::Append)) return false;
		file.write(msg.toByteArray().data());
		file.flush();

		const auto maxlogfilesize = qApp->property("AppConfig").toHash()
			.value("MaxLogfileSize", 10).toUInt() * 1024 * 1024;
		if (file.size() >= maxlogfilesize) {
			file.close();
			auto fileName = file.fileName();
			fileName.chop(4);
			const auto index = fileName.lastIndexOf('_');
			fileName = fileName.left(index + 1)
				+ QString::number(fileName.right(fileName.size() - index - 1).toInt() + 1) + ".txt";
			file.setFileName(fileName);
		}
		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////

	QString GetQMessageLogContext(const QMessageLogContext & context)
	{
		QString r;
		if (context.file != nullptr && strlen(context.file) > 0) {
			r.push_back(QFileInfo(context.file).fileName());
			r.push_back(':');
		}
		if (context.line > 0) {
			r.push_back(QString::number(context.line));
			r.push_back(':');
		}
		if (context.function != nullptr && strlen(context.function) > 0) {
			r.push_back(context.function);
			r.push_back(':');
		}
		return r;
	}

	void QtMessageHandler_AsLocalFile(QtMsgType mType, const QMessageLogContext & context, const QString & msg)
	{
		if (qApp == nullptr) return;
		static std::once_flag of;
		static QPointer<QObject> obj(new ShellObject(&SaveLog));
		std::call_once(of, [&]() {
			auto thread = new QThread(qApp);
			thread->connect(qApp, &QCoreApplication::aboutToQuit, thread, &QThread::quit);
			thread->start();

			obj->moveToThread(thread);
			obj->connect(thread, &QThread::finished, obj, &QObject::deleteLater);
		});
		static const char* types[] = { "D", "W", "C", "F", "I", "S" };
		const auto data = QString("%1[%2][%3] %4%5\n")
			.arg(QDateTime::currentDateTime().toString("yy/MM/dd hh:mm:ss:zzz"))
			.arg("0x" + QString::number(reinterpret_cast<qlonglong>(QThread::currentThreadId()), 16))
			.arg(types[mType])
			.arg(GetQMessageLogContext(context))
			.arg(msg);
		if (obj != nullptr) {
			QMetaObject::invokeMethod(obj, "execute", Qt::QueuedConnection, Q_ARG(const QVariant&, data));
		}
	}

	bool LoadAppConfig()
	{
		if (qApp == nullptr) return false;
		const auto configFilePath = GetAppConfigPath() + "/app.json";
		QFile file(configFilePath);
		if (!file.open(QIODevice::ReadOnly)) return false;
		const auto appConfig = QJsonDocument::fromJson(file.readAll()).toVariant().toMap();
		qApp->setProperty("AppConfig", appConfig);
		file.close();

		if (appConfig.value("LogLocalFile", false).toBool()) {
			qInstallMessageHandler(QtMessageHandler_AsLocalFile);
		}
		return true;
	}

	bool LoadAppResource()
	{
		bool r(true);
		const auto entryInfoList = QDir(GetAppResourcePath())
			.entryInfoList({ "*.rcc" }, QDir::Filter::Files);
		for (const auto& entryInfo : entryInfoList) {
			r = r && QResource::registerResource(entryInfo.absoluteFilePath());
		}
		return !entryInfoList.empty() && r;
	}

	bool LoadAppCommonQSS()
	{
		if (qApp == nullptr) return false;
		QFile file(":/stylesheet/app.qss");
		if (file.open(QIODevice::ReadOnly)) {
			qApp->setStyleSheet(file.readAll());
			file.close();
			return true;
		}
		return false;
	}

	bool LoadAppTranslator()
	{
		auto translator = new QTranslator(qApp);
		if (translator->load("app-zh_CN", GetAppTranslationsPath())) {
			qApp->installTranslator(translator);
		}
		return true;
	}

	bool InitializeApplication()
	{
		auto r = LoadAppResource();
		r = LoadAppCommonQSS() && r;
		r = LoadAppConfig() && r;
		r = LoadAppTranslator() && r;
		auto dbg = QGenericPluginFactory::create("uidbg", "");
		QObject::connect(qApp, &QApplication::aboutToQuit, dbg, &QObject::deleteLater);
		return r;
	}

	QWidget* ShowAppWindow()
	{
		QString widgetName;
		xutility::GetAppConfigValue(widgetName, "AppWindow.name");
		auto widget(Factory<QWidget>::produce(widgetName.toStdString()));
		if (widget != nullptr) {
			widget->setAttribute(Qt::WA_DeleteOnClose);
			QMetaObject::invokeMethod(widget, "initialize");
			widget->show();
			if (qApp != nullptr) {
				qApp->setProperty("AppWindow", QVariant::fromValue(widget));
			}
			int screen = -1;
			xutility::GetAppConfigValue(screen, "AppWindow.screen");
			xutility::AnchorOnParentWidget(widget, Qt::AlignCenter, screen);
		}
		return widget;
	}

	QVariantMap GetAppConfig()
	{
		return qApp->property("AppConfig").toMap();
	}

	void AnchorOnParentWidget(QWidget * widget, Qt::Alignment alignment, int screen)
	{
		do {
			if (widget == nullptr) break;
			auto dest = widget->parentWidget() == nullptr ?
				QApplication::desktop()->screen(screen) : widget->parentWidget();
			auto x = 0, y = 0;
			if (alignment & Qt::AlignHCenter) {
				x = dest->width() - widget->width();
				x /= 2;
			} else if (alignment & Qt::AlignRight) {
				x = dest->width() - widget->width();
			}

			if (alignment & Qt::AlignVCenter) {
				y = dest->height() - widget->height();
				y /= 2;
			} else if (alignment & Qt::AlignBottom) {
				y = dest->height() - widget->height();
			}
			QPoint base(x, y);
			if (widget->isWindow()) {
				base = dest->mapToGlobal(base);
			}
			widget->move(base);
		} while (false);
	}

	bool HasMethod(const QMetaObject * meta, const char * name)
	{
		if (meta == nullptr)
			return false;

		static QHash<QString, bool> s_cache;
		QString key(QByteArray(meta->className()));
		key.append('@').append(name);
		if (s_cache.contains(key))
			return s_cache[key];

		bool r(false);
		while (!r && meta != nullptr) {
			for (auto i = meta->methodOffset(), ic = meta->methodCount(); i < ic; ++i) {
				auto&& method = meta->method(i);
				if (method.name() == name) {
					r = true;
					break;
				}
			}
			meta = meta->superClass();
		}
		s_cache[key] = r;
		return r;
	}
/*
	QList<QVariantList> readCSV(const QString& filePath)
	{
		QList<QVariantList> res;
		QFile file(filePath);
		if (!file.open(QIODevice::ReadOnly)) return res;
		while (!file.atEnd()) {
			auto lineData = QString::fromUtf8(file.readLine()).split(',');
			if (lineData.isEmpty()) continue;
			QVariantList rowData;
			for (const auto& data : lineData) {
				rowData.push_back(data);
			}
			res.push_back(rowData);
		}
		file.close();
		return res;
	}

	QList<QVariantList> readXLS_X(const QString& filePath, const QString& suffix, int sheetIndex)
	{
		QList<QVariantList> result;
		do {
			std::unique_ptr<libxl::Book, std::function<void(libxl::Book*)>> book(
				suffix == "xls" ? xlCreateBook() : (suffix == "xlsx" ? xlCreateXMLBook() : nullptr)
				, [](libxl::Book* book) {
				if (book != nullptr) {
					book->release();
					book = nullptr;
				}
			});
			if (!book) {
				qLogInfo << "CreateBook Failed";
				break;
			}

			book->setKey(L"GCCG", L"windows-282123090cc0e6036db16b60a1j3k0h9");
			if (!book->load(filePath.toStdWString().data())) {
				qLogInfo << QString("book->load(%1) failed").arg(filePath);
				break;
			}
			if (book->sheetCount() < sheetIndex + 1) {
				qLogInfo << "sheetCount < sheetIndex + 1";
				break;
			}
			auto sheet = book->getSheet(sheetIndex);
			if (sheet == nullptr) {
				qLogInfo << "sheet(" << sheetIndex << ") is null";
				break;
			}

			const auto beginRow = sheet->firstRow();
			const auto beginColumn = sheet->firstCol();
			const auto endRow = sheet->lastRow();
			const auto endColumn = sheet->lastCol();
			if (beginRow < 0 || beginRow >= endRow - 1) {
				qLogInfo << "row:" << beginRow << "-" << endRow;
				break;
			}
			if (beginColumn < 0 || beginColumn >= endColumn) {
				qLogInfo << "column:" << beginColumn << "-" << endColumn;
				break;
			}

			for (auto r = beginRow; r < endRow; ++r) {
				QVariantList rowData;
				for (auto c = beginColumn; c < endColumn; ++c) {
					QVariant value;
					auto cellType = sheet->cellType(r, c);
					if (cellType == libxl::CellType::CELLTYPE_STRING) {
						value = QString::fromWCharArray(sheet->readStr(r, c));
					} else if (cellType == libxl::CellType::CELLTYPE_NUMBER) {
						value = sheet->readNum(r, c);
					} else if (cellType == libxl::CellType::CELLTYPE_BOOLEAN) {
						value = sheet->readBool(r, c);
					}
					rowData.push_back(value);
				}
				result.push_back(rowData);
			}
		} while (false);
		return result;
	}

	QList<QVariantList> ReadExcel(const QString & filePath, int sheetIndex)
	{
		QList<QVariantList> result;
		do {
			QFileInfo fileInfo(filePath);
			const auto suffix = fileInfo.completeSuffix();
			if (suffix.isEmpty()) break;
			if (suffix == "xlsx" || suffix == "xls") {
				return readXLS_X(filePath, suffix, sheetIndex);
			} else if (suffix == "csv") {
				return readCSV(filePath);
			}
		} while (false);
		return result;
	}

	QList<QList<QVariantList>> ReadExcel(const QString & filePath)
	{
		QList<QList<QVariantList>>r;
		do {
			const auto suffix = filePath.mid(filePath.lastIndexOf('.') + 1);
			std::unique_ptr<libxl::Book, std::function<void(libxl::Book*)>> book(
				suffix == "xls" ? xlCreateBook() : (suffix == "xlsx" ? xlCreateXMLBook() : nullptr)
				, [](libxl::Book* book) {
				if (book != nullptr) {
					book->release();
					book = nullptr;
				}
			});
			if (!book) {
				qLogInfo << "CreateBook Failed";
				break;
			}

			book->setKey(L"GCCG", L"windows-282123090cc0e6036db16b60a1j3k0h9");
			if (!book->load(filePath.toStdWString().data())) {
				qLogInfo << QString("book->load(%1) failed").arg(filePath);
				break;
			}
			const auto sheetCount = book->sheetCount();

			for (auto i = 0; i < sheetCount; ++i) {
				QList<QVariantList> result;
				auto sheet = book->getSheet(i);
				if (sheet == nullptr) {
					qLogInfo << "sheet(" << i << ") is null";
					break;
				}

				const auto beginRow = sheet->firstRow();
				const auto beginColumn = sheet->firstCol();
				const auto endRow = sheet->lastRow();
				const auto endColumn = sheet->lastCol();
				if (beginRow < 0 || beginRow >= endRow - 1) {
					qLogInfo << "row:" << beginRow << "-" << endRow;
					break;
				}
				if (beginColumn < 0 || beginColumn >= endColumn) {
					qLogInfo << "column:" << beginColumn << "-" << endColumn;
					break;
				}

				for (auto r = beginRow; r < endRow; ++r) {
					QVariantList rowData;
					for (auto c = beginColumn; c < endColumn; ++c) {
						QVariant value;
						auto cellType = sheet->cellType(r, c);
						if (cellType == libxl::CellType::CELLTYPE_STRING) {
							value = QString::fromWCharArray(sheet->readStr(r, c));
						} else if (cellType == libxl::CellType::CELLTYPE_NUMBER) {
							value = sheet->readNum(r, c);
						} else if (cellType == libxl::CellType::CELLTYPE_BOOLEAN) {
							value = sheet->readBool(r, c);
						}
						rowData.push_back(value);
					}
					result.push_back(rowData);
				}
				r.push_back(result);
			}
		} while (false);
		return r;
	}

	bool WriteExcel(const QString & filePath, const QStringList & sheetNameList, const QList<QList<QVariantList>>& dataList)
	{
		bool r(false);
		do {
			const auto suffix = filePath.mid(filePath.lastIndexOf('.') + 1);
			std::unique_ptr<libxl::Book, std::function<void(libxl::Book*)>> book(
				suffix == "xls" ? xlCreateBook() : (suffix == "xlsx" ? xlCreateXMLBook() : nullptr)
				, [](libxl::Book* book) {
				if (book != nullptr) {
					book->release();
					book = nullptr;
				}
			});
			if (!book) {
				qDebug() << "CreateBook Failed";
				break;
			}
			book->setKey(L"GCCG", L"windows-282123090cc0e6036db16b60a1j3k0h9");
			for (auto i = dataList.begin(); i != dataList.end(); ++i) {
				const auto distance = std::distance(dataList.begin(), i);
				 auto sheetName = (distance >= sheetNameList.size() || sheetNameList[distance].trimmed().isEmpty())
					 ? QString("Sheet%1").arg(distance) : sheetNameList[distance];
				 auto sheet = book->addSheet(sheetName.toStdWString().c_str());
				 if (!sheet) continue;
				 for (auto r = 0; r < i->size(); ++r) {
					 for (auto c = 0; c < (*i)[r].size(); ++c) {
						 sheet->writeStr(r, c, (*i)[r][c].toString().toStdWString().c_str());
					 }
				 }
			}
			r = book->save(filePath.toStdWString().c_str());
		} while (false);
		return r;
	}
*/
	void OpenExplorerFile(const QString & file)
	{
#if (defined WIN32) || (defined _WIN32)
		QStringList param;
		if (!QFileInfo(file).isDir()) {
			param.push_back("/select,");
		}
		param.push_back(QDir::toNativeSeparators(file));
		QProcess::startDetached("explorer", param);
#endif
	}

	void SetItemData(QAbstractItemModel * m, int row, int column, const QVariant & display
		, const QMap<int, QVariant>& extral_default, const QModelIndex & parent)
	{
		auto&& index = m->index(row, column, parent);
		m->setItemData(index, extral_default);
		m->setData(index, display, Qt::DisplayRole);

		if (!extral_default.contains(Qt::TextAlignmentRole))
			m->setData(index, Qt::AlignCenter, Qt::TextAlignmentRole);

		if (!extral_default.contains(Qt::ToolTipRole))
			m->setData(index, display, Qt::ToolTipRole);
	}
}
