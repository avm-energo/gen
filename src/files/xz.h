#ifndef XZ_H
#define XZ_H

#include <QString>

#define LOG_MAX_SIZE 1048576

class XZ
{
public:
    XZ();
    static void checkNGzip(QString &fileName);
    static bool rotateGzipLogs(const QString &path);
};

#endif // XZ_H
