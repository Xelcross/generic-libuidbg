#include "consolewidget.h"

namespace dbg {
template <class _Ty, size_t _Size>
constexpr size_t ArraySize(const _Ty (&)[_Size]) noexcept {
    return _Size;
}
}

static ConsoleWidget* Self = nullptr;

struct { QtMsgType type; const char* name; } MsgTypeNames[] = {
    { QtDebugMsg, "Debug" },
    { QtWarningMsg, "Warning" },
    { QtCriticalMsg, "Critical" },
    { QtFatalMsg, "Fatal" },
    { QtInfoMsg, "Info" },
};

void messageHandler(QtMsgType type, const QMessageLogContext & context, const QString & text)
{
    if (Self == nullptr) return;
    auto log = QString("[%1][%2]: %3")
        .arg(QDateTime::currentDateTime().toString("yy-mm-dd hh:mm:ss"))
        .arg(MsgTypeNames[type].name)
        .arg(text);
    QMetaObject::invokeMethod(Self, "writeLog", Qt::QueuedConnection, Q_ARG(int, type), Q_ARG(const QString&, log));
}

struct ConsoleWidget::Data
{
    ConsoleWidget* q;
    QPushButton* buttons[3];
    QCheckBox* checkers[5];
    QTextBrowser* browser;
    QtMessageHandler oldmsgHanlder;


    Data()
        : q(nullptr)
        , browser(nullptr)
    {
        memset(buttons, 0, sizeof(buttons));
        memset(checkers, 0, sizeof(checkers));
        oldmsgHanlder = qInstallMessageHandler(messageHandler);
    }

    ~Data()
    {
      qInstallMessageHandler(oldmsgHanlder);
    }

    void init(ConsoleWidget* ot)
    {
        if (q != nullptr) return;
        q = ot;
        q->resize(600, 600);
        auto layout = new QVBoxLayout(q);
        {
            auto hlayout = new QHBoxLayout;
            {
                auto vlayout = new QVBoxLayout;
                for (auto i = 0; i < dbg::ArraySize(checkers); ++i) {
                    auto& checker = checkers[i];
                    vlayout->addWidget(checker = new QCheckBox(MsgTypeNames[i].name, q));
                    checker->setObjectName(checker->text());
                    checker->setChecked(true);
                }
                vlayout->addStretch();
                hlayout->addLayout(vlayout);
            }

            {
                hlayout->addWidget(browser = new QTextBrowser(q));
                browser->setTextColor(Qt::white);
                browser->setContextMenuPolicy(Qt::NoContextMenu);
                browser->setReadOnly(true);
                browser->setStyleSheet("QTextBrowser{ background: black; }");
            }

            layout->addLayout(hlayout);
        }

        {
            auto hlayout = new QHBoxLayout;
            hlayout->addStretch();
            const QString names[] = {"ObjTree", "Clear", "Close"};
            for (auto i = 0; i < dbg::ArraySize(buttons); ++i) {
                auto& button = buttons[i];
                hlayout->addWidget(button = new QPushButton(names[i], q));
                button->setObjectName(button->text());
            }
            QObject::connect(buttons[1], &QPushButton::clicked, browser, &QTextBrowser::clear);
            QObject::connect(buttons[2], &QPushButton::clicked, q, &QWidget::close);
            layout->addLayout(hlayout);
        }
    }

    void writeLog(int type, const QString& log)
    {
        do {
            if (!q->isVisible()) break;
            if (type < 0 || type >= dbg::ArraySize(checkers)) break;
            if (!checkers[type]->isChecked()) break;
            browser->append(log);
        } while (false);
    }
};

ConsoleWidget::ConsoleWidget(QWidget *parent)
    : QWidget(parent)
    , d(new ConsoleWidget::Data)
{
    d->init(this);
    Self = this;
}

ConsoleWidget::~ConsoleWidget()
{
    Self = nullptr;
}

void ConsoleWidget::writeLog(int type, const QString &log)
{
    d->writeLog(type, log);
}

DEFINE_PRODUCTIONLINE_DEFAULT(QWidget, ConsoleWidget, QWidget*)
