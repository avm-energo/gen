#pragma once

#include <cstdint>
#include <gen/gen_export.h>

constexpr unsigned int UINT24_MAX = 16777215;

class GENLIB_EXPORT uint24 final
{
private:
    constexpr inline static std::size_t elements = 3;
    std::uint8_t value[elements];

public:
    explicit uint24() noexcept;
    explicit uint24(const uint24 &val) noexcept;
    explicit uint24(const std::uint32_t val) noexcept;

    uint24 &operator=(const uint24 &input) noexcept;
    uint24 &operator=(const std::uint32_t input) noexcept;

    operator std::uint32_t() const noexcept;
    operator std::uint16_t() const noexcept;

    uint24 operator+(const uint24 &val) const noexcept;
    uint24 operator-(const uint24 &val) const noexcept;
    uint24 operator*(const uint24 &val) const noexcept;
    uint24 operator/(const uint24 &val) const noexcept;
    uint24 &operator+=(const uint24 &val) noexcept;
    uint24 &operator-=(const uint24 &val) noexcept;
    uint24 &operator*=(const uint24 &val) noexcept;
    uint24 &operator/=(const uint24 &val) noexcept;

    operator bool() const noexcept;
    bool operator!() const noexcept;
    bool operator==(const uint24 &val) const noexcept;
    bool operator!=(const uint24 &val) const noexcept;
    bool operator>=(const uint24 &val) const noexcept;
    bool operator<=(const uint24 &val) const noexcept;
};
