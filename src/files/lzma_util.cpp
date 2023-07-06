#include <QDebug>
#include <gen/files/lzma_util.h>

constexpr auto bufferSize = BUFSIZ;

LzmaUtil::LzmaUtil(token token) : state(LZMA_OK), action(LZMA_RUN), inbuf(BUFSIZ, 0x00), outbuf(BUFSIZ, 0x00)
{
}

void LzmaUtil::lzmaProcess(QByteArray &input, QByteArray &output, LzmaBaseStream *stream, bool isDecompress)
{
    stream->cleanInput();
    stream->setOutput(outbuf);
    while (state != LZMA_STREAM_END)
    {
        // Fill the input buffer if it is empty.
        if (stream->isInputEmpty() && !input.isEmpty())
        {
            inbuf = input.left(bufferSize);
            stream->setInput(inbuf);
            input.remove(0, bufferSize);
            if (input.isEmpty())
                action = LZMA_FINISH;
        }
        state = lzma_code(stream->getStream(), action);
        if (stream->isOutputEmpty() || (state == LZMA_STREAM_END))
        {
            /*! When lzma_code() has returned LZMA_STREAM_END,
             *  the output buffer is likely to be only partially
             *  full. Calculate how much new data there is to
             *  be written to the output file.
             */
            auto write_size = outbuf.size() - stream->getStream()->avail_out;
            outbuf.resize(static_cast<int>(write_size));
            output.append(outbuf);
            outbuf.resize(bufferSize);
            stream->setOutput(outbuf);
        }
        if (isDecompress && state != LZMA_OK && state != LZMA_STREAM_END)
        {
            qCritical("Decompress error!");
            break;
        }
    }
    state = LZMA_OK;
    action = LZMA_RUN;
}

QByteArray LzmaUtil::compress(QByteArray &input)
{
    QByteArray out;
    std::unique_ptr<LzmaEncodeStream> stream(new LzmaEncodeStream());
    lzmaProcess(input, out, stream.get(), false);
    return out;
}

QByteArray LzmaUtil::decompress(QByteArray &input)
{
    QByteArray out;
    std::unique_ptr<LzmaDecodeStream> stream(new LzmaDecodeStream());
    lzmaProcess(input, out, stream.get(), true);
    return out;
}
