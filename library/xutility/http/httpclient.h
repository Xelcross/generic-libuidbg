#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include "../xutility_global.h"
#include "lite/pridata.h"
#include <QtNetwork/QtNetwork>

class XUTILITY_EXPORT HttpClient : public QObject
{
	PriData
	Q_OBJECT
public:
	explicit HttpClient(QObject* parent = nullptr);
	HttpClient& operator=(const HttpClient& client);
	~HttpClient();

	void setDefaultHost(const QString& host);
	void setDefaultRetryTimes(unsigned int);

	static QString queryParam(const QVariant& param);
	static QString queryParam(const QVariantMap& param);

	bool execute(const QString& route
		, const QString& method
		, const QByteArray& body = QByteArray()
		, std::function<void(int stauts, const QByteArray& body)>&& replyAction = nullptr);

	bool execute(const QString& route
		, QNetworkAccessManager::Operation operation
		, const QByteArray& body = QByteArray()
		, std::function<void(int stauts, const QByteArray& body)>&& replyAction = nullptr);

	bool execute(const QString& route
		, const QString& method
		, const QVariantMap& header = QVariantMap()
		, const QByteArray& body = QByteArray()
		, std::function<void(int status, const QString& err, const QByteArrayList& header, const QByteArray& body)>&& replyAction = nullptr);

	bool execute(const QString& route
		, QNetworkAccessManager::Operation operation
		, const QVariantMap& header = QVariantMap()
		, const QByteArray& body = QByteArray()
		, std::function<void(int status, const QString& err, const QByteArrayList& header, const QByteArray& body)>&& replyAction = nullptr);
};

#endif //HTTPCLIENT_H