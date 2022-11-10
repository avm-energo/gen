#include "gen/logger.h"

#include "gen/errorqueue.h"
#include "gen/files.h"
#include "gen/stdfunc.h"

#include <iostream>

struct MsgDescr
{
    Logger::LogLevels loglevel;
    const char *prefix;
};
const static QMap<QtMsgType, MsgDescr> msgTypes {
    { QtDebugMsg, { Logger::LogLevels::LOGLEVEL_DEBUG, "[DEBUG]" } },      //
    { QtWarningMsg, { Logger::LogLevels::LOGLEVEL_WARN, "[WARNING]" } },   //
    { QtCriticalMsg, { Logger::LogLevels::LOGLEVEL_CRIT, "[CRITICAL]" } }, //
    { QtFatalMsg, { Logger::LogLevels::LOGLEVEL_FATAL, "[FATAL]" } },      //
    { QtInfoMsg, { Logger::LogLevels::LOGLEVEL_INFO, "[INFO]" } }          //
};

static QString logfilename = "coma.log"; // имя по умолчанию
Logger::LogLevels Logger::_logLevel = Logger::LogLevels::LOGLEVEL_WARN;

void Logger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (_logLevel < msgTypes.value(type).loglevel)
        return;
    const char space = ' ';
    //    const char colon = ':';
    QStringList buffer = QString(context.file).split("\\");
    QString sourceFile = buffer.isEmpty() ? "" : buffer.takeLast();
    QFile logFile;
    QTextStream out;

    std::string function = context.function ? context.function : "";
    std::string rubbish(" __cdecl");
    StdFunc::RemoveSubstr(function, rubbish);

    ErrorMsg tmpm {
        QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss"), // DateTime
        type,                                                         // Msg type
        sourceFile,                                                   // File
        context.line,                                                 // Line
        msg                                                           // Message
    };
    ErrorQueue::GetInstance().pushError(tmpm);

    logFile.setFileName(logfilename);
    out.setDevice(&logFile);
    logFile.open(QFile::ReadWrite | QFile::Text | QFile::Append);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz "); // Log datetime
    out << msgTypes.value(type).prefix << space << msg << Qt::endl;
    out.flush(); // Flush buffer
    // logFile.close();
    Files::checkNGzip(&logFile);
}

void Logger::writeStart(const QString &filename)
{
    logfilename = filename;
    QFile logFile(logfilename);
    Files::makePath(logFile);
    QTextStream out;
    out.setDevice(&logFile);
    logFile.open(QFile::ReadWrite | QFile::Text | QFile::Append);
    out << "=====================================\nLog file started at "
        << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") + "\n"
        << QCoreApplication::applicationName() << " v." << QCoreApplication::applicationVersion();
    out.flush();
    // logFile.close();
    Files::checkNGzip(&logFile);
}

void Logger::setLogLevel(LogLevels level)
{
    _logLevel = level;
}

/// Категории мы сейчас не используем, задел на будущее
Q_LOGGING_CATEGORY(logDebug, "Debug")
Q_LOGGING_CATEGORY(logInfo, "Info")
Q_LOGGING_CATEGORY(logWarning, "Warning")
Q_LOGGING_CATEGORY(logCritical, "Critical")
Q_LOGGING_CATEGORY(logFatal, "Fatal")
