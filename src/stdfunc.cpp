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
#include <QTextStream>
#include <QThread>
#include <QTimer>
#include <gen/error.h>
#include <gen/pch.h>
#include <gen/stdfunc.h>

QString StdFunc::HomeDir = "";       // Рабочий каталог программы
QString StdFunc::SystemHomeDir = ""; // Системный каталог программы
decltype(StdFunc::state) StdFunc::state {};

QString StdFunc::DeviceIP = "";
QString StdFunc::s_OrganizationString = "";
int StdFunc::m_tuneRequestCount = 0;

// clang-format off
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QtCore/QTextCodec>
#include <QRegExpValidator>
#else
#include <QtCore5Compat/QTextCodec>
#include <QRegularExpressionValidator>
#endif
// clang-format on

/*! \brief Initialization function for static class fields.
 *  \details Initialize next fields by values: system home directory, organization, device IP, etc...
 */
void StdFunc::Init()
{
    SystemHomeDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/"
        + QCoreApplication::applicationName() + "/";
    if ((!SystemHomeDir.contains("/root")) && SystemHomeDir.startsWith("//"))
    {
        if (SystemHomeDir.front() == '/')
            SystemHomeDir.replace(0, 1, "/root");
    }

    QDir dir(SystemHomeDir);
    if (!dir.exists())
        dir.mkdir(SystemHomeDir);
    auto sets = std::unique_ptr<QSettings>(new QSettings);
    SetOrganizationString(sets->value("OrganizationString", "Р&К").toString());
    SetDeviceIP(sets->value("DeviceIP", "172.16.11.12").toString());
    SetTuneRequestCount(sets->value("TuneRequestCount", "20").toInt());
}

/// \brief Converts a version from quint32 datatype to string view.
QString StdFunc::VerToStr(quint32 num)
{
    int mv = (num & 0xFF000000) >> 24;
    int lv = (num & 0x00FF0000) >> 16;
    int sv = (num & 0x0000FFFF);
    QString tmpString
        = QString::number(mv, 10) + "." + QString::number(lv, 10) + "-" + QString("%1").arg(sv, 4, 10, QChar('0'));
    return tmpString;
}

/// \brief Converts a version from string view to quint32 datatype.
quint32 StdFunc::StrToVer(const QString &str)
{
    auto dotPos = str.indexOf('.');
    auto dashPos = str.indexOf('-');
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    int mv = str.leftRef(dotPos).toInt() << 24;
    int lv = str.midRef(dotPos + 1, dashPos - dotPos - 1).toInt() << 16;
    int sv = str.rightRef(str.size() - dashPos - 1).toInt();
#else
    // Don't trust clazy: Qt6 QString hasn't leftRef, midRef and rightRef methods
    int mv = str.left(dotPos).toInt() << 24;
    int lv = str.mid(dotPos + 1, dashPos - dotPos - 1).toInt() << 16;
    int sv = str.right(str.size() - dashPos - 1).toInt();
#endif
    return (mv | lv | sv);
}

/// \brief Converts a value from string view to float point datatype.
float StdFunc::ToFloat(const QString &text, bool *ok)
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

/*! \brief Checks that the number is in the specified interval.
 *  \param var Checked number.
 *  \param base,tolerance Limits of specified interval.
 */
bool StdFunc::FloatIsWithinLimits(double var, double base, double tolerance)
{
    auto tmpf = fabs(var - base);
    return (tmpf < fabs(tolerance));
}

/// \brief Sets new path for home directory field.
void StdFunc::SetHomeDir(const QString &dir)
{
    HomeDir = dir;
}

/// \brief Returns path for home directory.
QString StdFunc::GetHomeDir()
{
    return HomeDir;
}

/// \brief Returns path for system home directory.
QString StdFunc::GetSystemHomeDir()
{
    return SystemHomeDir;
}

/*! \brief Sets new device's IP.
 *  \param ip String that contains new IP address.
 */
void StdFunc::SetDeviceIP(const QString &ip)
{
    DeviceIP = ip;
    auto sets = std::unique_ptr<QSettings>(new QSettings);
    sets->setValue("DeviceIP", ip);
}

/// \brief Returns device's IP.
QString StdFunc::ForDeviceIP()
{
    return DeviceIP;
}

/*! \brief Sets new organization name.
 *  \param str New organization name in string view.
 */
void StdFunc::SetOrganizationString(const QString &str)
{
    s_OrganizationString = str;
    auto sets = std::unique_ptr<QSettings>(new QSettings);
    sets->setValue("OrganizationString", str);
}

/// \brief Returns organization name.
QString StdFunc::OrganizationString()
{
    return s_OrganizationString;
}

/// \brief Sets new tune request count.
void StdFunc::SetTuneRequestCount(int n)
{
    m_tuneRequestCount = n;
}

/// \brief Returns tune request count.
int StdFunc::TuneRequestCount()
{
    return m_tuneRequestCount;
}

/// \brief Sets cancel state when enabled.
void StdFunc::Cancel()
{
    if (state.cancelEnabled)
        state.cancelled = true;
}

/// \brief Turns off cancel state.
void StdFunc::ClearCancel()
{
    state.cancelled = false;
}

/// \brief Returns cancel state.
bool StdFunc::IsCancelled()
{
    return state.cancelled;
}

/// \brief Disallows to set cancel state.
void StdFunc::SetCancelDisabled()
{
    state.cancelEnabled = false;
}

/// \brief Allows to set cancel state.
void StdFunc::SetCancelEnabled()
{
    state.cancelEnabled = true;
}

/*! \brief Returns the position of first bit set.
 *  \details Returns position of the first '1' starting from LSB.
 *  \param dword 32bit bitstring.
 */
int StdFunc::IndexByBit(quint32 dword)
{
    quint32 bit = 0x00000001;
    for (int i = 0; i < 32; ++i)
        if (dword & bit)
            return (i + 1);
    return 0;
}

/*! \brief Returns the 32bit bitstring by index position.
 *  \details Returns 32bit bitstring with '1' in index position.
 *  \param index Position of '1' from LSB.
 *  \return Example: 0 => 0, 1 => 1, 2 => 2, 3 => 4, ...
 */
quint32 StdFunc::BitByIndex(int index)
{
    quint32 bit = 0x00000001;
    if ((index == 0) || (index > 31))
        return 0;
    return (bit << (index - 1));
}

/// \brief Puts the thread to sleep for a given time in ms.
void StdFunc::Wait(int ms)
{
    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(ms);
    loop.exec();

    /*
    QElapsedTimer tmr;
    tmr.start();
    while (tmr.elapsed() < ms)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        QThread::msleep(MAINSLEEP);
    }
    */
}

/*! \brief Ping IP address, return IP address if host is alive or return 0 if host is dead
 *  \details Platform dependent ping function, ping IP address through cmdline utility,
 *  parse cmd output. If output contains TTL host is alive else host is dead.
 *  \return IP address if host is alive, otherwise returns 0.
 */
quint32 StdFunc::Ping(quint32 addr)
{
    QString exec = "ping";
#ifdef Q_OS_WINDOWS
    QString param = "-n";
#endif
#ifdef Q_OS_LINUX
    QString param = "-c";
#endif
    QHostAddress host(addr);
    auto pingProcess = new QProcess;
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

/*! \brief Checks port and IPv4 address for connection.
 *  \details Checks if the connection can be made with given IP address and port.
 *  \param ip4Addr[in] IPv4 host address.
 *  \param port[in] Connection port.
 *  \return IPv4 address if connection can be made, 0 otherwise.
 */
quint32 StdFunc::CheckPort(quint32 ip4Addr, quint16 port)
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

/*! \brief Removes specified substring from specified string.
 *  \param str[in, out] The string from which the substring will be removed.
 *  \param substr[i] The substring that will be removed from string.
 */
void StdFunc::RemoveSubstr(std::string &str, std::string &substr)
{
    std::string::size_type n = substr.length();
    for (std::string::size_type i = str.find(substr); i != std::string::npos; i = str.find(substr))
        str.erase(i, n);
}

QValidator *StdFunc::getRegExpValidator(const QString &pattern, QObject *parent)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // Qt5 code
    auto validator = new QRegExpValidator(QRegExp(pattern), parent);
#else
    // Qt6 code
    auto validator = new QRegularExpressionValidator(QRegularExpression(pattern), parent);
#endif
    return validator;
}
