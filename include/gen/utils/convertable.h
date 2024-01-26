#pragma once

#include <QVariant>
#include <cstdint>
#include <gen/gen_export.h>

namespace utils
{

class GENLIB_EXPORT Convertable
{
private:
    QVariant m_data;

public:
    explicit Convertable(const QVariant &data) noexcept;
    explicit Convertable(QVariant &&data) noexcept;
    explicit Convertable(const Convertable &rhs) noexcept;
    explicit Convertable(Convertable &&rhs) noexcept;

    Convertable &operator=(const Convertable &rhs) noexcept;
    Convertable &operator=(Convertable &&rhs) noexcept;

    operator bool() const noexcept;
    operator QString() const noexcept;

    operator std::int16_t() const noexcept;
    operator std::uint16_t() const noexcept;
    operator std::int32_t() const noexcept;
    operator std::uint32_t() const noexcept;
    operator std::int64_t() const noexcept;
    operator std::uint64_t() const noexcept;
};

} // namespace utils
