#include "xz.h"

#include "../error.h"

XzCompressor::XzCompressor(token token, const int preset) : stream(LZMA_STREAM_INIT), state(LZMA_OK)
{
    state = lzma_easy_encoder(&stream, preset, LZMA_CHECK_CRC64);
    if (state != LZMA_OK)
        ERMSG("Something wrong with lzma_easy_encoder");
}

XzCompressor::~XzCompressor()
{
    lzma_end(&stream);
}

void XzCompressor::endFileWork()
{
    state = LZMA_OK;
    stream.avail_in = 0;
    stream.avail_out = 0;
    stream.next_in = nullptr;
    stream.next_out = nullptr;
    stream.total_in = 0;
    stream.total_out = 0;
}

QByteArray XzCompressor::compress(const QByteArray &input)
{
    QByteArray in = input, out;
    QByteArray inbuf, outbuf;
    outbuf.resize(BUFSIZ);
    stream.next_in = NULL;
    stream.avail_in = 0;
    stream.next_out = reinterpret_cast<uint8_t *>(outbuf.data());
    stream.avail_out = outbuf.size();
    lzma_action action = LZMA_RUN;
    while (true)
    {
        // Fill the input buffer if it is empty.
        if ((stream.avail_in == 0) && !in.isEmpty())
        {
            inbuf = in.left(BUFSIZ);
            stream.next_in = reinterpret_cast<uint8_t *>(inbuf.data());
            stream.avail_in = inbuf.size();
            in.remove(0, BUFSIZ);
            if (in.isEmpty())
                action = LZMA_FINISH;
        }
        state = lzma_code(&stream, action);
        if ((stream.avail_out == 0) || (state == LZMA_STREAM_END))
        {
            /*! When lzma_code() has returned LZMA_STREAM_END,
             *  the output buffer is likely to be only partially
             *  full. Calculate how much new data there is to
             *  be written to the output file.
             */
            size_t write_size = outbuf.size() - stream.avail_out;
            outbuf.resize(static_cast<int>(write_size));
            out.append(outbuf);
            outbuf.resize(BUFSIZ);
            stream.next_out = reinterpret_cast<uint8_t *>(outbuf.data());
            stream.avail_out = outbuf.size();
        }
        if (state != LZMA_OK)
        {
            /*! Once everything has been encoded successfully, the
             *  return value of lzma_code() will be LZMA_STREAM_END.
             *  It is important to check for LZMA_STREAM_END. Do not
             *  assume that getting ret != LZMA_OK would mean that
             *  everything has gone well.
             */
            if (state == LZMA_STREAM_END)
                break;
        }
    }
    endFileWork();
    return out;
}
