#include "gen/logger.h"

#include "gen/errorqueue.h"
#include "gen/files.h"
#include "gen/stdfunc.h"

#include <iostream>

const static QMap<QtMsgType, const char *> msgTypes {
    { QtDebugMsg, "[DEBUG]" },       //
    { QtWarningMsg, "[WARNING]" },   //
    { QtCriticalMsg, "[CRITICAL]" }, //
    { QtFatalMsg, "[FATAL]" },       //
    { QtInfoMsg, "[INFO]" }          //
};

static QString logfilename = "coma.log"; // имя по умолчанию

void Logger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
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
    out << msgTypes.value(type) << space << msg << Qt::endl;
    out.flush(); // Flush buffer
    // logFile.close();
    Files::checkNGzip(&logFile);
}

void Logger::writeStart(const QString &filename)
{
    logfilename = filename;
    QFile logFile(logfilename);
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

/// Категории мы сейчас не используем, задел на будущее
Q_LOGGING_CATEGORY(logDebug, "Debug")
Q_LOGGING_CATEGORY(logInfo, "Info")
Q_LOGGING_CATEGORY(logWarning, "Warning")
Q_LOGGING_CATEGORY(logCritical, "Critical")
