#include "commandcenter.h"
#include "abstractcommand.h"
#include "lite/factory.h"
#include "lite/qtlog.h"
#include "../utility/tools.h"

Q_GLOBAL_STATIC(CommandCenter, commandCenter);

struct CommandCenter::Data {
	QMap<QString, QPointer<AbstractCommand>> commandHashMap;
	QThread thread;

	QPointer<AbstractCommand> command(const QString & commandType)
	{
		const auto commandTypeRef = commandType.trimmed();
		if (commandTypeRef.trimmed().isEmpty()) return nullptr;
		auto& cmd = commandHashMap[commandTypeRef];
		do {
			if (!cmd.isNull()) break;
			cmd = Factory<AbstractCommand>::produce(commandTypeRef.toStdString());

			if (cmd.isNull()) break;

			QVariantMap properties;
			if (!xutility::GetAppConfigValue(properties, "CommandProperty." + commandTypeRef)) break;
			for (auto i = properties.begin(); i != properties.end(); ++i) {
				cmd->setProperty(i.key().toStdString().data(), i.value());
			}

			if (!cmd->runInMainThread()) {
				if (!thread.isRunning())
					thread.start();
				cmd->moveToThread(&thread);
				QObject::connect(qApp, &QCoreApplication::aboutToQuit, cmd.data(), &QObject::deleteLater);
			} else {
				cmd->setParent(qApp);
			}

			CommandCenter::instance()->linkUi(cmd.data());
		} while (false);
		return cmd;
	}
};

CommandCenter::CommandCenter()
	: d(new CommandCenter::Data)
{
}

CommandCenter::~CommandCenter()
{
	d->thread.quit();
	d->thread.wait(3000);
}

CommandCenter * CommandCenter::instance()
{
	return commandCenter();
}

void CommandCenter::execute(const QString & commandType, const QString & name, const QVariant & param)
{
	if (auto command = d->command(commandType)) {
		QMetaObject::invokeMethod(command.data()
			, "execute"
			, Q_ARG(const QString&, name)
			, Q_ARG(const QVariant&, param)
		);
	}
}

void CommandCenter::linkUi(const QString & commandType, QObject * ui)
{
	if (ui == nullptr) return;
	auto command = d->command(commandType);
	if (command.isNull()) return;
	command->linkUi(ui);
}

void CommandCenter::linkUi(QObject * ui)
{
	if (ui == nullptr) return;
	QStringList commandTypeList;
	if (!xutility::GetAppConfigValue(commandTypeList, QString("Command.") + ui->metaObject()->className()))
		return;

	for (auto& commandType : commandTypeList) {
		linkUi(commandType, ui);
	}
}
