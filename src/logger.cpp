#include "logger.h"

#include "errorqueue.h"
#include "files/files.h"
#include "stdfunc.h"

#include <iostream>

#define LOGFILE "coma.log"

const static QMap<QtMsgType, const char *> msgTypes {
    { QtDebugMsg, "[DEBUG]" },       //
    { QtWarningMsg, "[WARNING]" },   //
    { QtCriticalMsg, "[CRITICAL]" }, //
    { QtFatalMsg, "[FATAL]" },       //
    { QtInfoMsg, "[INFO]" }          //
};

void Logger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    const char space = ' ';
    //    const char colon = ':';
    QStringList buffer = QString(context.file).split("\\");
    QString sourceFile = buffer.isEmpty() ? "" : buffer.takeLast();
    QString fileName(StdFunc::GetSystemHomeDir() + LOGFILE);
    QFile logFile;
    QTextStream out;

    std::string function = context.function ? context.function : "";
    std::string rubbish(" __cdecl");
    StdFunc::removeSubstr(function, rubbish);

    ErrorMsg tmpm {
        QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss"), // DateTime
        type,                                                         // Msg type
        sourceFile,                                                   // File
        context.line,                                                 // Line
        msg                                                           // Message
    };
    ErrorQueue::GetInstance().pushError(tmpm);

    logFile.setFileName(fileName);
    out.setDevice(&logFile);
    logFile.open(QFile::Append | QFile::Text);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz "); // Log datetime
    out << msgTypes.value(type) << space << msg << Qt::endl;
    out.flush(); // Flush buffer
    logFile.close();
    Files::checkNGzip(fileName);
}

void Logger::writeStart()
{
    QString fileName(StdFunc::GetSystemHomeDir() + LOGFILE);
    QFile logFile(fileName);
    QTextStream out;
    out.setDevice(&logFile);
    logFile.open(QFile::Append | QFile::Text);
    out << "=====================================\nLog file started at "
        << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") + "\n"
        << QCoreApplication::applicationName() << " v." << QCoreApplication::applicationVersion();
    out.flush();
    logFile.close();
    Files::checkNGzip(fileName);
}

/// Категории мы сейчас не используем, задел на будущее
Q_LOGGING_CATEGORY(logDebug, "Debug")
Q_LOGGING_CATEGORY(logInfo, "Info")
Q_LOGGING_CATEGORY(logWarning, "Warning")
Q_LOGGING_CATEGORY(logCritical, "Critical")
