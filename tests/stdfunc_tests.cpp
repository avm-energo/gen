#include "gen_test_class.h"

#include <QDebug>
#include <gen/stdfunc.h>

void GenTestClass::stdToByteArrayTest01()
{
    constexpr quint16 data = 32125;
    QByteArray expected;
    expected.append(static_cast<char>(data % 0x100));
    expected.append(static_cast<char>(data / 0x100));
    auto actual = StdFunc::toByteArray(data);
    QCOMPARE(actual, expected);
}

void GenTestClass::stdToByteArrayTest02()
{
    constexpr quint16 data = 12205;
    QByteArray expected;
    expected.append(static_cast<char>(data & 0xFF)); // low
    expected.append(static_cast<char>(data >> 8));   // high
    auto actual = StdFunc::toByteArray(data);
    QCOMPARE(actual, expected);
}

void GenTestClass::stdToByteArrayTest03()
{
    constexpr quint32 data = 0xaabbccdd;
    auto first = StdFunc::toByteArray(data);
    auto second = StdFunc::toByteArray(qToBigEndian(data));
    qDebug() << first << second;
    QVERIFY(first != second);
}

void GenTestClass::stdToByteArrayTest04()
{
    constexpr quint64 data = 0xaabbccddaabbccdd;
    auto first = StdFunc::toByteArray(data);
    auto second = StdFunc::toByteArray(qToBigEndian(data));
    StdFunc::Wait();
    // qDebug() << first << second;
    QVERIFY(first != second);
}

void GenTestClass::stdSafeMemoryCopyTest01()
{
    const QByteArray data = QByteArrayLiteral("\x64\x32\xff\x00\x00\x00\x00\x00\x00\x00");
    constexpr std::uint32_t expected = 0x0000ff32;
    std::uint64_t actual = 0;
    StdFunc::safeMemoryCopy(actual, data, 1);
    // qDebug() << actual << expected;
    QCOMPARE(actual, expected);
}

void GenTestClass::stdSafeMemoryCopyTest02()
{
    const QByteArray data = QByteArrayLiteral("\x00\xff\x00\xdd\xcc\xbb\xaa\xdd\xcc\xbb\xaa\x00\xff");
    constexpr std::uint64_t expected = 0xaabbccddaabbccdd;
    std::uint64_t actual = 0;
    StdFunc::safeMemoryCopy(actual, data, 3);
    // qDebug() << actual << expected;
    QCOMPARE(actual, expected);
}

void GenTestClass::stdGetFromByteArrayTest01()
{
    const QByteArray data = QByteArrayLiteral("\x64\x32\xff\x00\x00\x00\x00\x00\x00\x00");
    constexpr std::uint32_t expected = 0x0000ff32;
    const auto actual = StdFunc::getFromByteArray<std::uint32_t>(data, 1);
    // qDebug() << actual << expected;
    QCOMPARE(actual, expected);
}

void GenTestClass::stdGetFromByteArrayTest02()
{
    const QByteArray data = QByteArrayLiteral("\x00\xff\x00\xdd\xcc\xbb\xaa\xdd\xcc\xbb\xaa\x00\xff");
    constexpr std::uint64_t expected = 0xaabbccddaabbccdd;
    const auto actual = StdFunc::getFromByteArray<std::uint64_t>(data, 3);
    // qDebug() << actual << expected;
    QCOMPARE(actual, expected);
}
