#pragma once

#include "error.h"
#include "gen_export.h"

#include <QFile>

class GENLIB_EXPORT Files
{
public:
    Files();
    static QString ChooseFileForSave(const QString &res, const QString &filenamestr = "");
    static Error::Msg LoadFromFile(const QString &filename, QByteArray &ba);
    static Error::Msg SaveToFile(const QString &filename, const QByteArray &src);
    static QStringList Drives();
    static QStringList SearchForFile(QStringList &di, const QString &filename, bool subdirs = false);
    static QString GetFirstDriveWithLabel(QStringList &filepaths, const QString &label);
    static void checkNGzip(QFile *logFile);
    static bool rotateGzipLogs(const QString &path);
    static void makePath(const QFile &path);
};
