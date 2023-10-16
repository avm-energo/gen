#include <QByteArray>
#include <gen/stdfunc.h>
#include <gen/uint24.h>

uint24::uint24() : value { 0, 0, 0 }
{
}

uint24::uint24(const uint24 &val)
{
    *this = val;
}

uint24::uint24(const std::uint32_t &val)
{
    *this = val;
}

uint24 &uint24::operator=(const uint24 &input)
{
    value[0] = input.value[0];
    value[1] = input.value[1];
    value[2] = input.value[2];
    return *this;
}

uint24 &uint24::operator=(const std::uint32_t input)
{
    value[0] = ((unsigned char *)&input)[0];
    value[1] = ((unsigned char *)&input)[1];
    value[2] = ((unsigned char *)&input)[2];
    return *this;
}

uint24::operator std::uint32_t() const
{
    /* Sign extend negative quantities */
    if (value[2] & 0x80)
        return (0xff << 24) | (value[2] << 16) | (value[1] << 8) | value[0];
    else
        return (value[2] << 16) | (value[1] << 8) | value[0];
}

uint24::operator std::uint16_t() const
{
    return (value[1] << 8) | value[0];
}

uint24 uint24::operator+(const uint24 &val) const
{
    return uint24((uint32_t) * this + (uint32_t)val);
}

uint24 uint24::operator-(const uint24 &val) const
{
    return uint24((uint32_t) * this - (uint32_t)val);
}

uint24 uint24::operator*(const uint24 &val) const
{
    return uint24((uint32_t) * this * (uint32_t)val);
}

uint24 uint24::operator/(const uint24 &val) const
{
    return uint24((uint32_t) * this / (uint32_t)val);
}

uint24 &uint24::operator+=(const uint24 &val)
{
    *this = *this + val;
    return *this;
}

uint24 &uint24::operator-=(const uint24 &val)
{
    *this = *this - val;
    return *this;
}

uint24 &uint24::operator*=(const uint24 &val)
{
    *this = *this * val;
    return *this;
}

uint24 &uint24::operator/=(const uint24 &val)
{
    *this = *this / val;
    return *this;
}

uint24::operator bool() const
{
    return static_cast<std::uint32_t>(*this) != 0;
}

bool uint24::operator!() const
{
    return !(static_cast<std::uint32_t>(*this));
}

bool uint24::operator==(const uint24 &val) const
{
    return static_cast<std::uint32_t>(*this) == static_cast<std::uint32_t>(val);
}

bool uint24::operator!=(const uint24 &val) const
{
    return static_cast<std::uint32_t>(*this) != static_cast<std::uint32_t>(val);
}

bool uint24::operator>=(const uint24 &val) const
{
    return static_cast<std::uint32_t>(*this) >= static_cast<std::uint32_t>(val);
}

bool uint24::operator<=(const uint24 &val) const
{
    return static_cast<std::uint32_t>(*this) <= static_cast<std::uint32_t>(val);
}
