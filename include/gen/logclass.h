#pragma once

#include <QByteArray>
#include <QFile>
#include <QMutex>
#include <gen/gen_export.h>

#define LOG_MAX_SIZE 1048576

namespace
{
constexpr char logStart[] = "=== Log started ===\n";
constexpr char logExt[] = "log";
}

enum class LogLevel : std::uint8_t
{
    Info = 0,
    Warning,
    Error,
    Undefined = std::numeric_limits<std::uint8_t>::max()
};

class GENLIB_EXPORT LogClass
{
private:
    bool m_canLog;
    QFile m_file;
    QMutex m_mutex;

public:
    explicit LogClass() noexcept;
    ~LogClass() noexcept;

    void init(const QString &filename);
    QString getFilename() const noexcept;

    void error(const QString &str);
    void warning(const QString &str);
    void info(const QString &str);
    void logging(const QString &message, const LogLevel level = LogLevel::Info);

    void intvarvalue(const QString &var, int value);
    void writeFile(const QString &msg, const QString &prepend = "");
    void writeRaw(const QByteArray &ba);
};
