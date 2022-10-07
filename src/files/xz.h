#ifndef XZ_H
#define XZ_H

#include "../gen_export.h"
#include "../singleton.h"

#include <QByteArray>
#include <lzma.h>

class GENLIB_EXPORT XzCompressor : public Singleton<XzCompressor>
{
private:
    lzma_stream stream;
    lzma_ret state;

    void endFileWork();

public:
    explicit XzCompressor(token token, const int preset = 6);
    ~XzCompressor();
    QByteArray compress(const QByteArray &input);
    // QByteArray decompress(const QByteArray &ba);
};

#endif // XZ_H
