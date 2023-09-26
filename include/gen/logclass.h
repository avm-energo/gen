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
    void error(const QString &str);
    void warning(const QString &str);
    void info(const QString &str);
    void intvarvalue(const QString &var, int value);
    void writeFile(const QString &msg, const QString &prepend = "");
    void writeRaw(const QByteArray &ba);
};
