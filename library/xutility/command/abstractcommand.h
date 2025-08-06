#ifndef ABSTRACTCOMMAND_H
#define ABSTRACTCOMMAND_H

#include "../xutility_global.h"
#include "../utility/tools.h"
#include "lite/pridata.h"

#include <QtCore/QtCore>

class HttpClient;

class XUTILITY_EXPORT AbstractCommand : public QObject
{
	PriData
	Q_OBJECT
    Q_PROPERTY(bool EnableHttpClient  READ enableHttpClient WRITE setEnableHttpClient)
    Q_PROPERTY(bool RunInMainThread  READ runInMainThread   WRITE setRunInMainThread)
public:
	explicit AbstractCommand(QObject* parent = nullptr);
	~AbstractCommand();

	void linkUi(QObject* ui);

	bool enableHttpClient() const;
	void setEnableHttpClient(bool enable);

	bool runInMainThread() const;
	void setRunInMainThread(bool value);

public slots:
	virtual bool execute(const QString& name, const QVariant& param) = 0;
	
signals:
	void sig_result(const QString& name, const QVariant& param, const QVariant& result, const QString& caller = QString());

protected:
	std::weak_ptr<HttpClient> httpClient() const;;
};


template <class Command>
class AbstractCommandX : public AbstractCommand
{
public:
	explicit AbstractCommandX(QObject* parent = nullptr) : AbstractCommand(parent)
	{
		static_assert(std::is_base_of<AbstractCommand, AbstractCommand>::value
			, "typename Command must be base of AbstractCommand!");
	}
	~AbstractCommandX() {}

protected:
	struct Result {
		QVariant result;
		std::shared_ptr<QEventLoop> eventloop_waitforfinished;
		QString caller;
	};
	using ResultPtr = std::shared_ptr<Result>;

	using Executor = bool(Command::*)(const QString& name, const QVariant& param, ResultPtr result);

	void registerExecutor(const QString& name, Executor executor)
	{
		executorHashMap[name.trimmed().toLower()] = executor;
	}

	bool execute(const QString& name, const QVariant& param) override
	{
		auto& executor = executorHashMap[name.trimmed().toLower()];
		if (executor == nullptr) return false;

		auto result = std::make_shared<Result>();
		if (auto caller = sender())
			result->caller = caller->metaObject()->className();
		if (qApp != nullptr && qApp->thread() == thread()) {
			bool isAsync(false);
			lite::GetJsonValue(isAsync, "async", param);
			if (!isAsync)
				result->eventloop_waitforfinished = std::make_shared<QEventLoop>();
		}

		auto ret = (static_cast<Command*>(this)->*executor)(name, param, result);
		if (ret && result->eventloop_waitforfinished) {
			result->eventloop_waitforfinished->exec();
		}
		return ret;
	}

	void commandResult(const QString& name, const QVariant& param, ResultPtr result)
	{
		if (!result) return;
		if (result->eventloop_waitforfinished && result->eventloop_waitforfinished->isRunning()) {
			result->eventloop_waitforfinished->quit();
		}
		emit sig_result(name, param, result->result, result->caller);
	}

private:
	QMap<QString, Executor> executorHashMap;
};


#endif //ABSTRACTCOMMAND_H