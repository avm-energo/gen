#pragma once

#include <QByteArray>
#include <gen/std_ext.h>
#include <vector>

namespace Files
{

/// \brief Zero-overhead представление структурированных бинарных файлов
template <typename BinaryRecord> //
class BinaryFile
{
public:
    typedef std_ext::remove_cvref_t<BinaryRecord> value_type;
    typedef value_type *iterator;
    typedef const value_type *const_iterator;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    template <typename Iterator> //
    struct Range
    {
        Iterator begin;
        Iterator end;
    };

private:
    QByteArray m_file;
    static constexpr inline auto s_recordSize = sizeof(value_type);

    inline void checking(const char *where) const noexcept
    {
        Q_ASSERT_X(m_file.size() % s_recordSize == 0, where, "Incorrect file size");
    }

    inline void removeImpl(iterator first, iterator last) noexcept
    {
        difference_type index = (first - begin()) * s_recordSize;
        difference_type length = (last - first) * s_recordSize;
        m_file.remove(index, length);
        checking("RemoveImpl calling");
    }

    inline void removeImpl(reverse_iterator first, reverse_iterator last) noexcept
    {
        difference_type index = (rend() - last) * s_recordSize;
        difference_type length = (last - first) * s_recordSize;
        m_file.remove(index, length);
        checking("RemoveImpl calling");
    }

    [[nodiscard]] inline bool moveImpl(iterator srcBegin, iterator srcEnd, iterator dst) noexcept
    {
        difference_type length = (srcEnd - srcBegin) * s_recordSize;
        difference_type index = (dst - begin()) * s_recordSize;
        index = ((dst - srcBegin) > 0) ? (index - length) : index;
        if ((srcBegin == begin() && srcEnd == end()) ||                //
            (dst >= srcBegin && dst <= srcEnd) || (dst == srcBegin) || //
            (dst == srcEnd) || (length <= 0) || (index < 0) ||         //
            (dst < begin()) || (dst > end()))
            return false;

        QByteArray tempRange(length, 0);
        std::copy(srcBegin, srcEnd, reinterpret_cast<iterator>(tempRange.data()));
        removeImpl(srcBegin, srcEnd);
        m_file.insert(index, tempRange);
        checking("moveImpl calling");
        return true;
    }

    [[nodiscard]] inline bool moveImpl( //
        reverse_iterator srcBegin,      //
        reverse_iterator srcEnd,        //
        reverse_iterator dst) noexcept  //
    {
        return false;
    }

public:
    explicit BinaryFile() noexcept = delete;

    explicit inline BinaryFile(const QByteArray &file) noexcept : m_file(file)
    {
        checking("BinaryFile c-tor calling");
    }

    explicit inline BinaryFile(QByteArray &&file) noexcept : m_file(std::move(file))
    {
        checking("BinaryFile c-tor calling");
    }

    inline size_type size() const noexcept
    {
        return m_file.size() / s_recordSize;
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
        return std::make_reverse_iterator(end());
    }

    inline reverse_iterator rend() noexcept
    {
        return std::make_reverse_iterator(begin());
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

    template <typename Iterator, typename Predicate> //
    static Range<Iterator> findRange(Iterator first, Iterator last, Predicate predicate) noexcept
    {
        bool firstFound = false;
        Range<Iterator> range { first, last };
        for (Iterator iter = first; iter != last; ++iter)
        {
            auto predicateResult { predicate(*iter) };
            if (predicateResult && !firstFound)
            {
                range.begin = iter;
                firstFound = true;
            }
            else if (!predicateResult && firstFound)
            {
                range.end = iter;
                break;
            }
        }
        return range;
    }

    template <typename Iterator, typename Predicate> //
    static std::vector<Range<Iterator>> findAllRanges(Iterator first, Iterator last, Predicate predicate) noexcept
    {
        std::vector<Range<Iterator>> ranges;
        ranges.reserve(8);
        while (first != last)
        {
            Range<Iterator> range { findRange(first, last, predicate) };
            if (range.begin == first && range.end == last)
                break;
            else
            {
                ranges.push_back(range);
                first = range.end;
            }
        }
        return ranges;
    }

    template <typename UnaryPredicate> //
    inline Range<iterator> findRange(UnaryPredicate p) noexcept
    {
        return BinaryFile<BinaryRecord>::findRange(begin(), end(), p);
    }

    template <typename UnaryPredicate> //
    inline Range<const_iterator> findRange(UnaryPredicate p) const noexcept
    {
        return BinaryFile<BinaryRecord>::findRange(begin(), end(), p);
    }

    template <typename UnaryPredicate> //
    inline std::vector<Range<iterator>> findAllRanges(UnaryPredicate p) noexcept
    {
        return BinaryFile<BinaryRecord>::findAllRanges(begin(), end(), p);
    }

    template <typename UnaryPredicate> //
    inline std::vector<Range<const_iterator>> findAllRanges(UnaryPredicate p) const noexcept
    {
        return BinaryFile<BinaryRecord>::findAllRanges(begin(), end(), p);
    }

    template <typename Iterator> //
    inline void remove(Iterator first, Iterator last) noexcept
    {
        removeImpl(first, last);
    }

    template <typename Iterator> //
    inline void remove(const Range<Iterator> &range) noexcept
    {
        remove(range.begin, range.end);
    }

    template <typename Iterator> //
    [[nodiscard]] inline bool move(Iterator srcBegin, Iterator srcEnd, Iterator dst) noexcept
    {
        return moveImpl(srcBegin, srcEnd, dst);
    }

    template <typename Iterator> //
    [[nodiscard]] inline bool move(const Range<Iterator> &range, Iterator dst) noexcept
    {
        return move(range.begin, range.end, dst);
    }
};

} // namespace Files
