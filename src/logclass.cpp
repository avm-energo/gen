#include <QDataStream>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QMutexLocker>
#include <gen/files.h>
#include <gen/logclass.h>
#include <gen/stdfunc.h>

LogClass::LogClass() noexcept : m_canLog(false)
{
}

LogClass::~LogClass() noexcept
{
    m_file.flush();
    m_file.close();
}

void LogClass::init(const QString &filename)
{
    // тестовая проверка открытия файла на запись
    m_file.setFileName(StdFunc::GetSystemHomeDir() + filename);
    if (!m_file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
    {
        m_canLog = false;
        qCritical("Ошибка открытия файла");
    }
    else
        m_canLog = true;
}

void LogClass::error(const QString &str)
{
    if (m_canLog)
        writeFile(str, "Error");
}

void LogClass::info(const QString &str)
{
    if (m_canLog)
        writeFile(str, "Info");
}

void LogClass::warning(const QString &str)
{
    if (m_canLog)
        writeFile(str, "Warning");
}

void LogClass::intvarvalue(const QString &var, int value)
{
    if (m_canLog)
        writeFile(QString::number(value), var);
}

void LogClass::writeFile(const QString &msg, const QString &prepend)
{
    QMutexLocker locker(&m_mutex);
    QString tmps = "[" + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz") + "] ";
    m_file.write(tmps.toLocal8Bit());
    if (!prepend.isEmpty())
        tmps = "[" + prepend + "] ";
    m_file.write(tmps.toLocal8Bit());
    m_file.write(msg.toLocal8Bit());
    m_file.write("\n");
    m_file.flush();
    Files::checkNGzip(&m_file);
}

// thread-safe function
void LogClass::writeRaw(const QByteArray &ba)
{
    QMutexLocker locker(&m_mutex);
    QString tmps = "[" + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz") + "] ";
    int writtenSize;
    writtenSize = m_file.write(tmps.toLocal8Bit());
    if (writtenSize == -1)
        return;
    writtenSize = m_file.write(ba);
    if (writtenSize == -1)
        return;
    if (!m_file.flush())
        return;
    Files::checkNGzip(&m_file);
}
