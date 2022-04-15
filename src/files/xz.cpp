#include "xz.h"

#include "../error.h"
#include "../stdfunc.h"

#include <QFile>
#include <QIODevice>
#include <QtDebug>

XZ::XZ()
{
}

void XZ::checkNGzip(QString &fileName)
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
    fileOut.write(StdFunc::compress(logFile.readAll()));
    fileOut.close();
    logFile.close();
}

bool XZ::rotateGzipLogs(const QString &path)
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
