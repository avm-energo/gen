#include "gen_test_class.h"

#include <QDebug>
#include <gen/stdfunc.h>

void GenTestClass::stdByteArrayTest01()
{
    constexpr quint16 data = 32125;
    QByteArray expected;
    expected.append(static_cast<char>(data % 0x100));
    expected.append(static_cast<char>(data / 0x100));
    auto actual = StdFunc::toByteArray(data);
    QCOMPARE(actual, expected);
}

void GenTestClass::stdByteArrayTest02()
{
    constexpr quint16 data = 12205;
    QByteArray expected;
    expected.append(static_cast<char>(data & 0xFF)); // low
    expected.append(static_cast<char>(data >> 8));   // high
    auto actual = StdFunc::toByteArray(data);
    QCOMPARE(actual, expected);
}

void GenTestClass::stdByteArrayTest03()
{
    constexpr quint32 data = 0xaabbccdd;
    auto first = StdFunc::toByteArray(data);
    auto second = StdFunc::toByteArray(qToBigEndian(data));
    qDebug() << first << second;
    QVERIFY(first != second);
}

void GenTestClass::stdByteArrayTest04()
{
    constexpr quint64 data = 0xaabbccddaabbccdd;
    auto first = StdFunc::toByteArray(data);
    auto second = StdFunc::toByteArray(qToBigEndian(data));
    StdFunc::Wait();
    qDebug() << first << second;
    QVERIFY(first != second);
}
