#include <QDebug>
#include <gen/files/lzma_stream.h>

LzmaBaseStream::LzmaBaseStream() : stream(LZMA_STREAM_INIT)
{
}

LzmaBaseStream::~LzmaBaseStream()
{
    lzma_end(&stream);
}

void LzmaBaseStream::setInput(QByteArray &input)
{
    stream.next_in = reinterpret_cast<uint8_t *>(input.data());
    stream.avail_in = input.size();
}

void LzmaBaseStream::setOutput(QByteArray &output)
{
    stream.next_out = reinterpret_cast<uint8_t *>(output.data());
    stream.avail_out = output.size();
}

void LzmaBaseStream::cleanInput()
{
    stream.next_in = nullptr;
    stream.avail_in = 0;
}

void LzmaBaseStream::cleanOutput()
{
    stream.next_out = nullptr;
    stream.avail_out = 0;
}

bool LzmaBaseStream::isInputEmpty() const
{
    return stream.avail_in == 0;
}

bool LzmaBaseStream::isOutputEmpty() const
{
    return stream.avail_out == 0;
}

lzma_stream *LzmaBaseStream::getStream()
{
    return &stream;
}

///////////////////

LzmaEncodeStream::LzmaEncodeStream(const uint32_t preset) : LzmaBaseStream()
{
    auto state = lzma_easy_encoder(&stream, preset, LZMA_CHECK_CRC32);
    if (state != LZMA_OK)
        qCritical("Something wrong with lzma_easy_encoder");
}

///////////////////

LzmaDecodeStream::LzmaDecodeStream(const uint64_t memlimit, const uint32_t flags) : LzmaBaseStream()
{
    auto state = lzma_stream_decoder(&stream, memlimit, flags);
    if (state != LZMA_OK)
        qCritical("Something wrong with lzma_stream_decoder");
}
