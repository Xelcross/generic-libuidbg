#include "httpclient.h"
#include "lite/qtlog.h"

struct HttpClient::Data {
	QString defaultHost;
	unsigned int defaultRetryTimes;
	QHash<QString, QNetworkAccessManager::Operation> methodHashMap;
	QNetworkAccessManager* networkAccessManager;

	Data() : networkAccessManager(nullptr), defaultRetryTimes(0) {}

	QString operationName(QNetworkAccessManager::Operation operation)
	{
		QString r;
		for (auto i = methodHashMap.begin(); i != methodHashMap.end(); ++i) {
			if (i.value() == operation) {
				r = i.key();
				break;
			}
		}
		return r;
	}

	QNetworkReply* executeRequest(const QNetworkRequest& req, QNetworkAccessManager::Operation operation, const QByteArray& byteArray)
	{
		QNetworkReply* reply(nullptr);
		switch (operation) {
		case QNetworkAccessManager::HeadOperation: reply = networkAccessManager->head(req); break;
		case QNetworkAccessManager::GetOperation: reply = networkAccessManager->get(req); break;
		case QNetworkAccessManager::PutOperation: reply = networkAccessManager->put(req, byteArray); break;
		case QNetworkAccessManager::PostOperation: reply = networkAccessManager->post(req, byteArray); break;
		case QNetworkAccessManager::DeleteOperation: reply = networkAccessManager->deleteResource(req); break;
		default: break;
		}
		return reply;
	}

	QString combineUrl(const QString& route)
	{
		QRegExp regexp("https?://", Qt::CaseInsensitive);
		if (route.indexOf(regexp) == 0) return route;

		QString r(defaultHost);
		if (!r.isEmpty() && !route.startsWith('/')) {
			r.append('/');
		}
		return r.append(route);
	}

	QNetworkReply* execute(const QNetworkRequest& req
		, QNetworkAccessManager::Operation operation
		, const QByteArray& byteArray
		, const std::function<void(int, const QString&, const QByteArrayList&, const QByteArray&)>& replyAction)
	{
		auto reply = executeRequest(req, operation, byteArray);
		if (reply != nullptr) {
			connect(reply, &QNetworkReply::finished, [=]() {
				const auto status = reply->attribute(
					QNetworkRequest::Attribute::HttpStatusCodeAttribute).toInt();
				auto retryTimes = reply->property("RetryTimes").toUInt();
				qLogInfo << retryTimes + 1 << ' '
					<< operationName(reply->operation()) << ' '
					<< reply->url().toString() << " reply:"
					<< "status:" << status << ' '
					<< reply->error() << ' '
//					<< byteArray << ' '
					<< reply->errorString();
				if (status != 200 && retryTimes < defaultRetryTimes) {
					const auto repeatReq = reply->request();
					auto repeatReply = execute(repeatReq, reply->operation(), byteArray, replyAction); 
					repeatReply->setProperty("RetryTimes", ++retryTimes);
				} else if (replyAction) {
					replyAction(status, reply->errorString(), reply->rawHeaderList(), reply->readAll());
				}
				reply->deleteLater();
			});
		}
		return reply;
	}
};

HttpClient::HttpClient(QObject* parent)
	: QObject(parent)
	, d(new HttpClient::Data)
{
	d->methodHashMap = {
		{ "HEAD", QNetworkAccessManager::HeadOperation },
		{ "GET", QNetworkAccessManager::GetOperation },
		{ "PUT", QNetworkAccessManager::PutOperation },
		{ "POST", QNetworkAccessManager::PostOperation },
		{ "DEL", QNetworkAccessManager::DeleteOperation },
	};
	d->networkAccessManager = new QNetworkAccessManager(this);
}

HttpClient& HttpClient::operator=(const HttpClient& client)
{
	setDefaultHost(client.d->defaultHost);
	setDefaultRetryTimes(client.d->defaultRetryTimes);
	return *this;
}

HttpClient::~HttpClient()
{
}

void HttpClient::setDefaultHost(const QString & host)
{
	d->defaultHost = host;
	if (!d->defaultHost.startsWith("http")) {
		d->defaultHost.prepend("http://");
	}
	if (d->defaultHost.endsWith('/')) {
		d->defaultHost.chop(1);
	}
}

void HttpClient::setDefaultRetryTimes(unsigned int retryTimes)
{
	d->defaultRetryTimes = retryTimes;
}

QString HttpClient::queryParam(const QVariant & param)
{
	return param.type() == QVariant::Map ?
		HttpClient::queryParam(reinterpret_cast<const QVariantMap&>(param))
		: "";
}

QString HttpClient::queryParam(const QVariantMap& param)
{
	QString query;
	for (auto i = param.begin(); i != param.end(); ++i) {
		if (!query.isEmpty()) {
			query.push_back('&');
		}
		query.push_back(i.key());
		query.push_back('=');
		query.push_back(i.value().toString());
	}
	return query;
}

bool HttpClient::execute(const QString & route
	, const QString & method
	, const QByteArray & body
	, std::function<void(int stauts, const QByteArray&body)>&& replyAction)
{
	return execute(route
		, d->methodHashMap.value(method.toUpper(), QNetworkAccessManager::UnknownOperation)
		, body
		, std::forward<decltype(replyAction)>(replyAction));
}

bool HttpClient::execute(const QString & route
	, QNetworkAccessManager::Operation operation
	, const QByteArray & body
	, std::function<void(int stauts, const QByteArray&body)>&& replyAction)
{
	return execute(route
		, operation
		, { { "Content-Type", "application/json" } }
		, body
		, [=](int status, const QString& err, const QByteArrayList& header, const QByteArray& body) {
		if (replyAction) replyAction(status, body);
	});
}

bool HttpClient::execute(const QString & route
	, const QString & method
	, const QVariantMap & header
	, const QByteArray & body
	, std::function<void(int, const QString&, const QByteArrayList&, const QByteArray&)>&& replyAction)
{
	return execute(route
		, d->methodHashMap.value(method.toUpper(), QNetworkAccessManager::UnknownOperation)
		, header
		, body
		, std::forward<decltype(replyAction)>(replyAction));
}

bool HttpClient::execute(const QString & route
	, QNetworkAccessManager::Operation operation
	, const QVariantMap & header
	, const QByteArray & body
	, std::function<void(int, const QString&, const QByteArrayList&, const QByteArray&)>&& replyAction)
{
	QNetworkRequest req(d->combineUrl(route));
	req.setAttribute(QNetworkRequest::CookieLoadControlAttribute, QNetworkRequest::Manual);
#ifndef QT_NO_SSL
	QSslConfiguration cf = req.sslConfiguration();
	cf.setPeerVerifyMode(QSslSocket::VerifyNone);
	req.setSslConfiguration(cf);
#endif //QT_NO_SSL

	for (auto i = header.begin(); i != header.end(); ++i) {
		req.setRawHeader(i.key().toUtf8(), i.value().toByteArray());
	}
	return d->execute(req, operation, body, replyAction) != nullptr;
}
