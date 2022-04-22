#ifndef XZ_H
#define XZ_H

#include <QString>

#define LOG_MAX_SIZE 1048576

class XZ
{
public:
    XZ();
    static QByteArray XZCompress(const QByteArray &ba);
    //    static QByteArray XZDecompress(QByteArray &ba);
};

#endif // XZ_H
