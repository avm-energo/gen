#pragma once
#include <cstddef>
#include <cstdint>
#include <typeinfo>

namespace ct
{
/// \brief From https://github.com/foonathan/string_id and ctti. All rights reserved.
/// \see http://www.isthe.com/chongo/tech/comp/fnv/#FNV-param
constexpr std::uint64_t fnv_basis = 14695981039346656037ull;
constexpr std::uint64_t fnv_prime = 1099511628211ull;

// FNV-1a 64 bit hash
constexpr std::uint64_t fnv1a_hash(std::size_t n, const char *str, std::uint64_t hash = fnv_basis)
{
    return n > 0 ? fnv1a_hash(n - 1, str + 1, (hash ^ *str) * fnv_prime) : hash;
}

template <std::size_t N> //
constexpr std::uint64_t fnv1a_hash(const char (&array)[N])
{
    return fnv1a_hash(N - 1, &array[0]);
}

template <typename T> struct hash
{
    constexpr auto operator()()
    {
        return fnv1a_hash(typeid(T).name());
    }
};

}
