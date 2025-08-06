#ifndef QTLOG_H
#define QTLOG_H

#if QT_VERSION_MAJOR >= 6
#else //QT_VERSION_MAJOR
#include <QDebug>
#include <QFileInfo>
#endif //QT_VERSION_MAJOR

#ifndef qSpotLog
#if (defined _DEBUG) || (defined DEBUG)
#define qSpotLog(qLogType) qLogType()
#else
#define qSpotLog(qLogType) qLogType() << (QFileInfo(__FILE__).fileName().toStdString()).c_str() << ":" << __LINE__ << ":" << __FUNCTION__ <<":"
#endif
#define qLogDebug       qSpotLog(qDebug)
#define qLogWarn        qSpotLog(qWarning)
#define qLogCritical    qSpotLog(qCritical)
#define qLogInfo        qSpotLog(qInfo)
#define qLogFatal       qSpotLog(qFatal)
#endif

#endif // QTLOG_H
