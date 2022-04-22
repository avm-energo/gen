#include "files.h"

#include "../board.h"
#include "../stdfunc.h"
#include "xz.h"

#include <QDirIterator>
#include <QFile>
#include <QIODevice>
#include <QStorageInfo>
#include <QtDebug>

Files::Files()
{
}

QString Files::ChooseFileForSave(const QString &ext, const QString &filenamestr)
{
    const auto &board = Board::GetInstance();
    QString MTypeM = (board.typeM() == 0) ? "00" : QString::number(board.typeM(), 16);
    QString tmps;
    if (filenamestr.isEmpty())
        tmps = StdFunc::GetHomeDir() + "/" + QString::number(board.typeB(), 16) + MTypeM + "-"
            + QString("%1").arg(board.serialNumber(Board::BaseMezz), 8, 10, QChar('0')) + "." + ext;
    else
        tmps = filenamestr;
    return tmps;
}

Error::Msg Files::LoadFromFile(const QString &filename, QByteArray &ba)
{
    if (filename.isEmpty())
    {
        qCritical("Пустое имя файла");
        return Error::Msg::FileNameError; // Пустое имя файла
    }
    std::unique_ptr<QFile> file = std::unique_ptr<QFile>(new QFile);
    file->setFileName(filename);
    if (!file->open(QIODevice::ReadOnly))
    {
        qCritical("Ошибка открытия файла");
        return Error::Msg::FileOpenError; // Ошибка открытия файла
    }
    ba = file->readAll();
    file->close();
    return Error::Msg::NoError;
}

Error::Msg Files::SaveToFile(const QString &filename, const QByteArray &src)
{
    if (filename.isEmpty())
        return Error::Msg::NoError; // Пустое имя файла
    std::unique_ptr<QFile> file = std::unique_ptr<QFile>(new QFile);
    file->setFileName(filename);
    if (!file->open(QIODevice::WriteOnly))
        return Error::Msg::FileOpenError; // Ошибка открытия файла
    if (file->write(src, src.size()) != -1)
    {
        // нет ошибок
        file->close();
        return Error::Msg::NoError;
    }
    else
    {
        // ошибка записи
        file->close();
        return Error::Msg::FileWriteError;
    }
}

QStringList Files::Drives()
{
    QStringList sl;
    QFileInfoList list = QDir::drives();
    for (const QFileInfo &fi : list)
        sl << fi.path();
    return sl;
}

QStringList Files::SearchForFile(QStringList &filepaths, const QString &filename, bool subdirs)
{
    QStringList files;
    for (const QString &filepath : filepaths)
    {
        QStringList sl = QStringList() << filename;
        QDirIterator it(filepath, sl, QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot,
            (subdirs) ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);
        while (it.hasNext())
            files << it.next();
    }
    return files;
}

QString Files::GetFirstDriveWithLabel(QStringList &filepaths, const QString &label)
{
    QString str = "";
    int i = 0;
    while ((str.isEmpty()) && (i < filepaths.size()))
    {
        QStorageInfo si;
        si.setPath(filepaths.at(i));
        if (si.name() == label)
            str = filepaths.at(i);
        ++i;
    }
    return str;
}

void Files::checkNGzip(QString &fileName)
{
    QFile logFile(fileName);
    if (logFile.size() < LOG_MAX_SIZE)
        return;
    if (!rotateGzipLogs(fileName))
        return;

    QFile fileOut;
    if (!logFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Cannot open the file" << fileName;
        return;
    }
    fileOut.setFileName(fileName + "0.gz");
    if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << "Cannot open the file" << fileOut.fileName();
        return;
    }
    fileOut.write(XZ::XZCompress(logFile.readAll()));
    fileOut.close();
    logFile.close();
}

bool Files::rotateGzipLogs(const QString &path)
{
    // rotating
    for (int i = 9; i > 0; --i)
    {
        QString tmpsnew = path + "." + QString::number(i) + ".gz";
        QString tmpsold = path + "." + QString::number(i - 1) + ".gz";
        QFile fn;
        fn.setFileName(tmpsnew);
        if (fn.exists())
            fn.remove();
        fn.setFileName(tmpsold);
        if (fn.exists())
        {
            if (fn.rename(tmpsnew) == false) // error
            {
                ERMSG("Cannot rename file");
                return false;
            }
        }
    }
    return true;
}
