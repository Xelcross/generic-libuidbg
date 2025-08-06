#include "abstractcommand.h"
#include "../http/httpclient.h"

struct AbstractCommand::Data {
	std::shared_ptr<HttpClient> client;
	bool runInMainThread;

	Data() : runInMainThread(true) {}
};

AbstractCommand::AbstractCommand(QObject * parent)
	: QObject(parent)
	, d(new AbstractCommand::Data)
{
}

AbstractCommand::~AbstractCommand()
{
}

void AbstractCommand::linkUi(QObject * ui)
{
	if (ui == nullptr) return;

	auto commands = ui->property("Command").toString();
	if (!commands.isEmpty())
		commands.append(',');
	commands.append(metaObject()->className());
	ui->setProperty("Command", commands);

	connect(ui, SIGNAL(sig_CommandExecute(const QString&, const QVariant&))
		, this, SLOT(execute(const QString&, const QVariant&)));

	connect(this, SIGNAL(sig_result(const QString&, const QVariant&, const QVariant&, const QString&))
		, ui, SLOT(onCommandExecuteResult(const QString&, const QVariant&, const QVariant&, const QString&)));
}

bool AbstractCommand::enableHttpClient() const
{
	return !!d->client;
}

void AbstractCommand::setEnableHttpClient(bool enable)
{
	if (!enable) {
		if (d->client)
			d->client.reset();
	} else if (!d->client) {
		d->client.reset(new HttpClient(this));

		QString host;
		xutility::GetAppConfigValue(host, "HttpClientHost");
		d->client->setDefaultHost(host);

		unsigned retryTimes(0);
		xutility::GetAppConfigValue(retryTimes, "HttpClientRetryTimes");
		d->client->setDefaultRetryTimes(retryTimes);
	}
}

bool AbstractCommand::runInMainThread() const
{
	return d->runInMainThread;
}

void AbstractCommand::setRunInMainThread(bool value)
{
	d->runInMainThread = value;
}

std::weak_ptr<HttpClient> AbstractCommand::httpClient() const
{
	return d->client;
}
