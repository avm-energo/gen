#pragma once

#include <QByteArray>
#include <QFile>

namespace Files
{

/// \brief Zero-overhead представление структурированных бинарных файлов
template <typename BinaryRecord> //
class BinaryFile
{
private:
    QByteArray m_file;

    inline void checking() const noexcept
    {
        Q_ASSERT_X(m_file.size() % sizeof(value_type) == 0, "BinaryFile c-tor", "Incorrect file size");
    }

public:
    typedef BinaryRecord value_type;
    typedef value_type *iterator;
    typedef const value_type *const_iterator;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef std::reverse_iterator<value_type> reverse_iterator;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    template <typename Iterator> //
    struct Scope
    {
        Iterator start;
        Iterator end;
    };

    explicit BinaryFile() noexcept = delete;

    explicit inline BinaryFile(const QByteArray &file) noexcept : m_file(file)
    {
        checking();
    }

    explicit inline BinaryFile(QByteArray &&file) noexcept : m_file(std::move(file))
    {
        checking();
    }

    inline size_type size() const noexcept
    {
        return m_file.size() / sizeof(value_type);
    }

    inline iterator begin() noexcept
    {
        return reinterpret_cast<iterator>(m_file.data());
    }

    inline iterator end() noexcept
    {
        return (begin() + size());
    }

    inline const_iterator begin() const noexcept
    {
        return reinterpret_cast<const_iterator>(m_file.constData());
    }

    inline const_iterator end() const noexcept
    {
        return (begin() + size());
    }

    inline reverse_iterator rbegin() noexcept
    {
        return end();
    }

    inline reverse_iterator rend() noexcept
    {
        return begin();
    }

    inline reference first() noexcept
    {
        return *begin();
    }

    inline const_reference first() const noexcept
    {
        return *begin();
    }

    inline reference last() noexcept
    {
        return *(begin() + (size() - 1));
    }

    inline const_reference last() const noexcept
    {
        return *(begin() + (size() - 1));
    }

    template <typename AnotherBinaryRecord> //
    inline BinaryFile<AnotherBinaryRecord> copyAs() const noexcept
    {
        return BinaryFile<AnotherBinaryRecord>(m_file);
    }
};

} // namespace Files
