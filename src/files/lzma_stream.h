#pragma once

#include "gen_export.h"

#include <QByteArray>
#include <lzma.h>

class GENLIB_EXPORT LzmaBaseStream
{
protected:
    lzma_stream stream;

public:
    explicit LzmaBaseStream();
    virtual ~LzmaBaseStream();
    void setInput(QByteArray &input);
    void setOutput(QByteArray &output);
    void cleanInput();
    void cleanOutput();
    bool isInputEmpty() const;
    bool isOutputEmpty() const;
    lzma_stream *getStream();
};

class GENLIB_EXPORT LzmaEncodeStream : public LzmaBaseStream
{
public:
    explicit LzmaEncodeStream(const uint32_t preset = LZMA_PRESET_DEFAULT);
};

class GENLIB_EXPORT LzmaDecodeStream : public LzmaBaseStream
{
public:
    explicit LzmaDecodeStream(const uint64_t memlimit = 0x20000000, const uint32_t flags = LZMA_CONCATENATED);
};
