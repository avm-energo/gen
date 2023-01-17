#pragma once
#include "gen_export.h"

#include <bitset>
#include <cstdint>

constexpr unsigned int UINT24_MAX = 16777215;

class GENLIB_EXPORT uint24
{
protected:
    unsigned char value[3];

public:
    uint24();
    uint24(const uint24 &val);
    uint24(const uint32_t &val);
    uint24 &operator=(const uint24 &input);
    uint24 &operator=(const uint32_t input);

    operator uint32_t() const;
    uint24 operator+(const uint24 &val) const;
    uint24 operator-(const uint24 &val) const;
    uint24 operator*(const uint24 &val) const;
    uint24 operator/(const uint24 &val) const;
    uint24 &operator+=(const uint24 &val);
    uint24 &operator-=(const uint24 &val);
    uint24 &operator*=(const uint24 &val);
    uint24 &operator/=(const uint24 &val);

    operator bool() const;
    bool operator!() const;
    bool operator==(const uint24 &val) const;
    bool operator!=(const uint24 &val) const;
    bool operator>=(const uint24 &val) const;
    bool operator<=(const uint24 &val) const;

    /* Define all operations you need below.. */
};

class ui24
{
    std::bitset<24> m_value;

public:
    ui24() = default;
    constexpr ui24(unsigned int value) noexcept : m_value { value }
    {
    }

    operator uint32_t() const
    {
        //  constexpr std::uint32_t negative_mask = (0xff << 24);
        return /*(m_value[23] ? negative_mask : 0) |*/ m_value.to_ulong();
    }
};
