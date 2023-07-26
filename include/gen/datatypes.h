#pragma once

#include <QVariant>
#include <array>
#include <gen/std_ext.h>
#include <gen/uint24.h>
#include <variant>

#ifdef __linux__
#include <ctime>
Q_DECLARE_METATYPE(timespec);
#endif

namespace DataTypes
{

enum Quality : quint8
{
    Bad = 8,
    Good = 192
};

enum DataBlockTypes
{
    BacBlock,
    BdaBlock,
    BdBlock,
    BciBlock
};

enum SignalTypes
{
    Float,
    BitString,
    FloatWithTime,
    SinglePointWithTime,
    ByteArray,
    File,
    DataRecVList,
    Block,
    OscillogramInfo,
    SwitchJournalInfo,
#ifdef __linux
    Timespec,
#endif
    GeneralResponse
};

enum GeneralResponseTypes
{
    Ok,
    Error,
    DataSize,
    DataCount
};

enum FileFormat : quint32
{
    Binary = 0,
    DefaultS2 = 1,
    CustomS2
};

struct BitStringStruct
{
    //    quint8 sigAdr[3];
    quint32 sigAdr;
    quint32 sigVal;
    quint8 sigQuality;
};
// первое - номера сигналов, второе - их значения ("" ~ недостоверное
// значение), третье - метка времени

struct FloatWithTimeStruct
{
    quint32 sigAdr;
    float sigVal;
    quint64 CP56Time;
    quint8 sigQuality;
};

struct FloatStruct
{
    quint32 sigAdr;
    float sigVal;
    quint8 sigQuality;
};

struct SinglePointWithTimeStruct
{
    quint32 sigAdr;
    quint8 sigVal;
    quint64 CP56Time;
    quint8 sigQuality;
};

struct BlockStruct
{
    quint32 ID;
    QByteArray data;
};

struct GeneralResponseStruct
{
    GeneralResponseTypes type;
    quint64 data;
};

typedef BlockStruct S2Record;
typedef BlockStruct HardwareStruct;
typedef QList<S2Record> S2FilePack;

struct SignalsStruct
{
    SignalTypes type;
    QVariant data;
};

struct Signal
{
    quint16 addr;
    quint16 value;
};

struct SingleCommand
{
    uint24 addr;
    bool value;
};

struct GasDensity
{
    quint32 TypeGaz; // Тип газа
    float MolW;      // Молярная масса, г/моль
    float Weight;    // Масса, кг
    float MolFrac;   // Мольная доля, %
};

using BYTE = uint8_t;
using WORD = uint16_t;
using DWORD = uint32_t;
using INT32 = int32_t;
using BYTE_4t = std::array<BYTE, 4>;
using WORD_4t = std::array<WORD, 4>;
using DWORD_4t = std::array<DWORD, 4>;
using BYTE_6t = std::array<BYTE, 6>;
using WORD_6t = std::array<WORD, 6>;
using DWORD_6t = std::array<DWORD, 6>;
using BYTE_8t = std::array<BYTE, 8>;
using WORD_8t = std::array<WORD, 8>;
using DWORD_8t = std::array<DWORD, 8>;
using BYTE_16t = std::array<BYTE, 16>;
using WORD_16t = std::array<WORD, 16>;
using DWORD_16t = std::array<DWORD, 16>;
using BYTE_32t = std::array<BYTE, 32>;
using WORD_32t = std::array<WORD, 32>;
using DWORD_32t = std::array<DWORD, 32>;
using FLOAT = float;
using FLOAT_2t = std::array<FLOAT, 2>;
using FLOAT_3t = std::array<FLOAT, 3>;
using FLOAT_4t = std::array<FLOAT, 4>;
using FLOAT_6t = std::array<FLOAT, 6>;
using FLOAT_8t = std::array<FLOAT, 8>;
using CONF_DENS = GasDensity;
using CONF_DENS_3t = std::array<CONF_DENS, 3>;

static_assert(sizeof(BYTE) != sizeof(WORD), "Broken datatypes");
static_assert(sizeof(BYTE) != sizeof(DWORD), "Broken datatypes");
static_assert(sizeof(INT32) == sizeof(DWORD), "Broken datatypes");
static_assert(sizeof(FLOAT) == sizeof(DWORD), "Broken datatypes");
static_assert(sizeof(WORD_4t) == sizeof(BYTE_8t), "Broken datatypes");
static_assert(sizeof(DWORD_4t) == sizeof(BYTE_16t), "Broken datatypes");
static_assert(sizeof(FLOAT_2t) == sizeof(BYTE_8t), "Broken datatypes");

using valueType = std::variant<BYTE, WORD, DWORD, INT32,     //
    BYTE_4t, WORD_4t, DWORD_4t,                              //
    BYTE_6t, WORD_6t, DWORD_6t,                              //
    BYTE_8t, WORD_8t, DWORD_8t,                              //
    BYTE_16t, WORD_16t, DWORD_16t,                           //
    BYTE_32t, WORD_32t, DWORD_32t,                           //
    FLOAT, FLOAT_2t, FLOAT_3t, FLOAT_4t, FLOAT_6t, FLOAT_8t, //
    CONF_DENS_3t>;
}

template <typename T> struct isValueType
{
    static constexpr bool value = std_ext::is_variant_alternative<T, DataTypes::valueType>();
};

Q_DECLARE_METATYPE(uint24);
Q_DECLARE_METATYPE(DataTypes::SingleCommand)
Q_DECLARE_METATYPE(DataTypes::BitStringStruct)
Q_DECLARE_METATYPE(DataTypes::FloatWithTimeStruct)
Q_DECLARE_METATYPE(DataTypes::FloatStruct)
Q_DECLARE_METATYPE(DataTypes::SinglePointWithTimeStruct)
Q_DECLARE_METATYPE(DataTypes::BlockStruct)
Q_DECLARE_METATYPE(DataTypes::S2FilePack)
Q_DECLARE_METATYPE(DataTypes::SignalsStruct)
Q_DECLARE_METATYPE(DataTypes::Signal)
Q_DECLARE_METATYPE(DataTypes::GeneralResponseStruct)
Q_DECLARE_METATYPE(DataTypes::GasDensity)
