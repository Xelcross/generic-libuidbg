#ifndef QTLOG_H
#define QTLOG_H

#if QT_VERSION_MAJOR >= 6
#else //QT_VERSION_MAJOR
#include <QDebug>
#endif //QT_VERSION_MAJOR

#include <QDateTime>

#define qSpotLog(qLogType) qLogType() << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ <<":"
#define qLogDebug       qSpotLog(qDebug)
#define qLogWarn        qSpotLog(qWarning)
#define qLogCritical    qSpotLog(qCritical)
#define qLogInfo        qSpotLog(qInfo)
#define qLogFatal       qSpotLog(qFatal)

#endif // QTLOG_H
