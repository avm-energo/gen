#include "xz.h"

#include "../error.h"

#include <lzma.h>

XZ::XZ()
{
}

QByteArray XZ::XZCompress(const QByteArray &ba)
{
    lzma_stream strm = LZMA_STREAM_INIT;
    lzma_ret ret = lzma_easy_encoder(&strm, 6, LZMA_CHECK_CRC64);
    if (ret != LZMA_OK)
    {
        ERMSG("Something wrong with lzma_easy_encoder");
        return QByteArray();
    }
    QByteArray inbufba, outbufba, outba;
    QByteArray inba = ba;
    strm.next_in = NULL;
    strm.avail_in = 0;
    outbufba.resize(BUFSIZ);
    strm.next_out = reinterpret_cast<uint8_t *>(outbufba.data());
    strm.avail_out = outbufba.size();
    lzma_action action = LZMA_RUN;
    while (true)
    {
        // Fill the input buffer if it is empty.
        if ((strm.avail_in == 0) && !inba.isEmpty())
        {
            strm.next_in = reinterpret_cast<uint8_t *>(inbufba.data());
            inbufba = inba.left(BUFSIZ);
            strm.avail_in = inbufba.size();
            inba.remove(0, BUFSIZ);
            if (inba.isEmpty())
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
