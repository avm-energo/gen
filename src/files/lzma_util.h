#pragma once

#include "../singleton.h"
#include "lzma_stream.h"

class GENLIB_EXPORT LzmaUtil : public Singleton<LzmaUtil>
{
private:
    lzma_ret state;
    lzma_action action;
    QByteArray inbuf, outbuf;

    void lzmaProcess(QByteArray &input, QByteArray &output, LzmaBaseStream *stream, bool isDecompress);

public:
    explicit LzmaUtil(token token);
    QByteArray compress(QByteArray &input);
    QByteArray decompress(QByteArray &input);
};
