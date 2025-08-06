#ifndef COMMANDCENTER_H
#define COMMANDCENTER_H

#include "../xutility_global.h"
#include "lite/pridata.h"

#include <QtCore/QtCore>

class XUTILITY_EXPORT CommandCenter
{
    PriData
public:
    CommandCenter();
    ~CommandCenter();

	static CommandCenter* instance();

    void execute(const QString& commandType, const QString& name, const QVariant& param);

    void linkUi(const QString& commandType, QObject* ui);
	void linkUi(QObject* ui);
};

#endif //COMMANDCENTER_H
