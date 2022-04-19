#include "files.h"

#include "../board.h"
#include "../stdfunc.h"

#include <QDirIterator>
#include <QFile>
#include <QStorageInfo>
#include <lzma.h>

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

QByteArray Files::XZCompress(QByteArray &ba)
{
    lzma_stream strm = LZMA_STREAM_INIT;
    lzma_ret ret = lzma_easy_encoder(&strm, 6, LZMA_CHECK_CRC64);
    if (ret != LZMA_OK)
    {
        ERMSG("Something wrong with lzma_easy_encoder");
        return QByteArray();
    }
    QByteArray inbufba, outbufba, outba;
    strm.next_in = NULL;
    strm.avail_in = 0;
    outbufba.resize(BUFSIZ);
    strm.next_out = reinterpret_cast<uint8_t *>(outbufba.data());
    strm.avail_out = outbufba.size();
    lzma_action action = LZMA_RUN;
    while (true)
    {
        // Fill the input buffer if it is empty.
        if ((strm.avail_in == 0) && !ba.isEmpty())
        {
            strm.next_in = reinterpret_cast<uint8_t *>(inbufba.data());
            inbufba = ba.left(BUFSIZ);
            strm.avail_in = inbufba.size();
            ba.remove(0, BUFSIZ);
            if (ba.isEmpty())
                action = LZMA_FINISH;
        }
        ret = lzma_code(&strm, action);
        if ((strm.avail_out == 0) || (ret == LZMA_STREAM_END))
        {
            // When lzma_code() has returned LZMA_STREAM_END,
            // the output buffer is likely to be only partially
            // full. Calculate how much new data there is to
            // be written to the output file.
            size_t write_size = outbufba.size() - strm.avail_out;
            outbufba.resize(static_cast<int>(write_size));
            outba.append(outbufba);
            strm.next_out = reinterpret_cast<uint8_t *>(outbufba.data());
            outbufba.resize(BUFSIZ);
            strm.avail_out = outbufba.size();
        }
        if (ret != LZMA_OK)
        {
            // Once everything has been encoded successfully, the
            // return value of lzma_code() will be LZMA_STREAM_END.
            //
            // It is important to check for LZMA_STREAM_END. Do not
            // assume that getting ret != LZMA_OK would mean that
            // everything has gone well.
            if (ret == LZMA_STREAM_END)
            {
                break;
            }
        }
    }
    return outba;
}
