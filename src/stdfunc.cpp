#include "stdfunc.h"

#include "error.h"
#include "pch.h"
#include "s2.h"

#include <QCoreApplication>
#include <QDataStream>
#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QHostAddress>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QTcpSocket>
#include <QTextCodec>
#include <QTextStream>
#include <QThread>
#include <QTimer>

QString StdFunc::HomeDir = "";       // рабочий каталог программы
QString StdFunc::SystemHomeDir = ""; // системный каталог программы

bool StdFunc::Cancelled = false;
bool StdFunc::s_cancelEnabled = true;
QString StdFunc::DeviceIP = "";
QString StdFunc::s_OrganizationString = "";
int StdFunc::m_tuneRequestCount = 0;

StdFunc::StdFunc()
{
}

void StdFunc::Init()
{

    SystemHomeDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/"
        + QCoreApplication::applicationName() + "/";
    if ((!SystemHomeDir.contains("/root")) && SystemHomeDir.startsWith("//"))
    {

        if (SystemHomeDir.front() == "/")
            SystemHomeDir.replace(0, 1, "/root");
    }

    QDir dir(SystemHomeDir);
    if (!dir.exists())
        dir.mkdir(SystemHomeDir);
    auto sets = std::unique_ptr<QSettings>(new QSettings);
    SetOrganizationString(sets->value("OrganizationString", "Р&К").toString());
    SetDeviceIP(sets->value("DeviceIP", "172.16.11.12").toString());
    setTuneRequestCount(sets->value("TuneRequestCount", "20").toInt());
}

QString StdFunc::VerToStr(quint32 num)
{
    int mv = (num & 0xFF000000) >> 24;
    int lv = (num & 0x00FF0000) >> 16;
    int sv = (num & 0x0000FFFF);
    QString tmpString
        = QString::number(mv, 10) + "." + QString::number(lv, 10) + "-" + QString("%1").arg(sv, 4, 10, QChar('0'));
    return tmpString;
}

quint32 StdFunc::StrToVer(const QString &str)
{
    auto dotPos = str.indexOf('.');
    int mv = str.leftRef(dotPos).toInt() << 24;
    auto dashPos = str.indexOf('-');
    int lv = str.midRef(dotPos + 1, dashPos - dotPos - 1).toInt() << 16;
    auto svStr = str.rightRef(str.size() - dashPos - 1);
    int sv = svStr.toInt();
    return mv | lv | sv;
}

float StdFunc::toFloat(const QString &text, bool *ok)
{
    bool floatok;
    float tmpf;
    tmpf = text.toFloat(&floatok);
    if (!floatok)
    {
        qCritical() << "Значение " << text << " не может быть переведено во float";
        if (ok != 0)
            *ok = false;
        return 0;
    }
    if (ok != 0)
        *ok = true;
    return tmpf;
}

void StdFunc::SetHomeDir(const QString &dir)
{
    HomeDir = dir;
}

QString StdFunc::GetHomeDir()
{
    return HomeDir;
}

QString StdFunc::GetSystemHomeDir()
{
    return SystemHomeDir;
}

void StdFunc::SetDeviceIP(const QString &ip)
{
    DeviceIP = ip;
    auto sets = std::unique_ptr<QSettings>(new QSettings);
    sets->setValue("DeviceIP", ip);
}

QString StdFunc::ForDeviceIP()
{
    return DeviceIP;
}

void StdFunc::SetOrganizationString(const QString &str)
{
    s_OrganizationString = str;
    auto sets = std::unique_ptr<QSettings>(new QSettings);
    sets->setValue("OrganizationString", str);
}

QString StdFunc::OrganizationString()
{
    return s_OrganizationString;
}

void StdFunc::setTuneRequestCount(int n)
{
    m_tuneRequestCount = n;
}

int StdFunc::tuneRequestCount()
{
    return m_tuneRequestCount;
}

void StdFunc::cancel()
{
    if (s_cancelEnabled)
        Cancelled = true;
}

void StdFunc::clearCancel()
{
    Cancelled = false;
}

bool StdFunc::isCancelled()
{
    return Cancelled;
}

void StdFunc::setCancelDisabled()
{
    s_cancelEnabled = false;
}

void StdFunc::setCancelEnabled()
{
    s_cancelEnabled = true;
}

int StdFunc::IndexByBit(quint32 dword)
{
    quint32 bit = 0x00000001;
    for (int i = 0; i < 32; ++i)
        if (dword & bit)
            return (i + 1);
    return 0;
}

quint32 StdFunc::BitByIndex(int idx)
{
    if ((idx == 0) || (idx > 31))
        return 0;
    return (0x00000001 << (idx - 1));
}

void StdFunc::Wait(int ms)
{
    QElapsedTimer tmr;
    tmr.start();
    while (tmr.elapsed() < ms)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        QThread::msleep(MAINSLEEP);
    }
}

/*!
     \brief Ping ip address, return ip address if host is alive or return 0 if host is dead

Platform dependent ping function, ping ip address through cmdline utility, parse cmd output.
If output contains TTL host is alive else host is dead.
Returns ip address if host is alive; otherwise returns 0.
    */
quint32 StdFunc::ping(quint32 addr)
{
    QString exec = "ping";
#ifdef Q_OS_WINDOWS
    QString param = "-n";
#endif
#ifdef Q_OS_LINUX
    QString param = "-c";
#endif
    QHostAddress host(addr);
    auto *pingProcess = new QProcess;
    QStringList params { param, "1", host.toString() };

    pingProcess->start(exec, params, QIODevice::ReadOnly);
    if (pingProcess->waitForFinished(100))
    {
        QTextCodec *codec = QTextCodec::codecForMib(2086);
        QString p_stdout = codec->toUnicode(pingProcess->readAllStandardOutput());
        QString p_stderr = codec->toUnicode(pingProcess->readAllStandardError());
#if QT_VERSION >= 0x050C00
        QStringList list = p_stderr.isEmpty() ? p_stdout.split("\r\n", Qt::SkipEmptyParts)
                                              : p_stderr.split("\r\n", Qt::SkipEmptyParts);
#else
        QStringList list = p_stderr.isEmpty() ? p_stdout.split("\r\n") : p_stderr.split("\r\n");
#endif
        if (std::any_of(list.constBegin(), list.constEnd(),
                [](const QString &i) { return i.contains("TTL", Qt::CaseInsensitive); }))
        {
            delete pingProcess;
            return addr;
        }
    }
    pingProcess->kill();
    pingProcess->waitForFinished();
    delete pingProcess;
    return 0;
}

/*! \brief Check port port for ip4Addr ip address

    \param ip address of host, ip4Addr, port for checking, port
    \return Ip address of host if port is open otherwise return 0;
*/
quint32 StdFunc::checkPort(quint32 ip4Addr, quint16 port)
{
    QHostAddress host(ip4Addr);
    QTcpSocket *sock = new QTcpSocket;
    QTimer *timer = new QTimer;
    timer->setInterval(1000);
    QEventLoop *loop = new QEventLoop;

    QObject::connect(sock, &QAbstractSocket::connected, [&]() { loop->quit(); });
    QObject::connect(timer, &QTimer::timeout, [&]() {
        loop->quit();
        ip4Addr = 0;
    });
    timer->start();
    // qDebug() << "Timer started";
    Q_ASSERT(sock != nullptr);
    sock->connectToHost(host, port);
    loop->exec(QEventLoop::ExcludeUserInputEvents);
    sock->disconnect();
    sock->disconnectFromHost();
    delete sock;
    timer->stop();
    delete timer;
    delete loop;
    qDebug() << (ip4Addr ? "Port opened" : "Port closed");
    return ip4Addr;
}

void StdFunc::removeSubstr(std::string &str, std::string &substr)
{
    std::string::size_type n = substr.length();
    for (std::string::size_type i = str.find(substr); i != std::string::npos; i = str.find(substr))
        str.erase(i, n);
}
