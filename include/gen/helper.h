#pragma once
#include "datatypes.h"
#include "gen_export.h"

#include <QDebug>
#include <bitset>

GENLIB_EXPORT QDebug operator<<(QDebug debug, const DataTypes::BitStringStruct &st);
GENLIB_EXPORT QDebug operator<<(QDebug debug, const DataTypes::FloatWithTimeStruct &st);
GENLIB_EXPORT QDebug operator<<(QDebug debug, const DataTypes::FloatStruct &st);
GENLIB_EXPORT QDebug operator<<(QDebug debug, const DataTypes::SinglePointWithTimeStruct &st);
GENLIB_EXPORT QDebug operator<<(QDebug debug, const DataTypes::BlockStruct &st);
GENLIB_EXPORT QDebug operator<<(QDebug debug, const DataTypes::SignalsStruct &st);
GENLIB_EXPORT QDebug operator<<(QDebug debug, const DataTypes::Signal &st);
GENLIB_EXPORT QDebug operator<<(QDebug debug, const DataTypes::GeneralResponseStruct &st);

template <std::size_t N> QDebug operator<<(QDebug debug, const std::bitset<N> &bitset)
{
    debug.nospace() << "bitset:";
    for (size_t i = 0; i < bitset.size(); i++)
        debug.nospace() << quint8(bitset.test(i));

    return debug.maybeSpace();
}
