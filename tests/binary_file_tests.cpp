#include "gen_test_class.h"

#include <gen/binary_file.h>

namespace detail
{

struct TestRecord1
{
    quint32 value_a;
    quint32 value_b;
    quint32 value_c;
    quint32 value_d;
};

struct TestRecord2
{
    quint64 value_a;
    quint64 value_b;
    quint64 value_c;
    quint64 value_d;
};

bool operator==(const TestRecord1 &lhs, const TestRecord1 &rhs)
{
    return (                          //
        lhs.value_a == rhs.value_a && //
        lhs.value_b == rhs.value_b && //
        lhs.value_c == rhs.value_c && //
        lhs.value_d == rhs.value_d    //
    );                                //
}

constexpr auto constDataSize = 12;
constexpr auto arrayDataSize = constDataSize * sizeof(TestRecord1);
constexpr auto maxValue = std::numeric_limits<quint32>::max();

static const TestRecord1 constData1[constDataSize] {
    { 1, 2, 3, 4 }, { 5, 6, 7, 8 }, { 9, 10, 11, 12 },          //
    { 13, 14, 15, 16 }, { 17, 18, 19, 20 }, { 21, 22, 23, 24 }, //
    { 25, 26, 27, 28 }, { 29, 30, 31, 32 }, { 33, 34, 35, 36 }, //
    { 37, 38, 39, 40 }, { 41, 42, 43, 44 }, { 45, 46, 47, 48 }  //
};

static const TestRecord1 constData2[constDataSize] {
    { 1, 2, 3, 4 }, { 5, 6, 7, 8 }, { 9, 10, 11, 12 },                   //
    { 13, 14, 15, 16 }, { 17, 18, 19, 20 }, { 21, 22, 23, 24 },          //
    { maxValue, 0, 1, 2 }, { 3, maxValue, 4, 5 }, { 6, 7, maxValue, 8 }, //
    { 37, 38, 39, 40 }, { 41, 42, 43, 44 }, { 45, 46, 47, 48 }           //
};

static const TestRecord1 constData3[constDataSize] {
    { 1, 2, maxValue, 4 }, { 5, maxValue, 7, 8 }, { 9, 10, 11, 12 },        //
    { 13, 14, 15, 16 }, { 17, maxValue, 19, 20 }, { maxValue, 22, 23, 24 }, //
    { maxValue, 0, 1, 2 }, { maxValue, 3, 4, 5 }, { 6, 7, 7, 8 },           //
    { 37, 38, 39, 40 }, { maxValue, 42, 43, 44 }, { maxValue, 46, 47, 48 }  //
};

bool testPredicate(const TestRecord1 &value)
{
    return (                         //
        value.value_a == maxValue || //
        value.value_b == maxValue || //
        value.value_c == maxValue || //
        value.value_d == maxValue    //
    );                               //
}

} // namespace detail

void GenTestClass::binaryFileCtorSizeTest()
{
    using namespace detail;
    using namespace Files;
    auto bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData1[0]), arrayDataSize);
    BinaryFile<TestRecord1> file1(bytes);
    QCOMPARE(file1.size(), constDataSize);
    BinaryFile<TestRecord1> file2(QByteArray::fromRawData( //
        reinterpret_cast<const char *>(&constData2[0]),    //
        arrayDataSize));                                   //
    QCOMPARE(file2.size(), constDataSize);
    BinaryFile<TestRecord2> file3(bytes);
    QCOMPARE(file3.size(), constDataSize / 2);
}

void GenTestClass::binaryFileIterTest()
{
    using namespace detail;
    using namespace Files;
    auto bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData1[0]), arrayDataSize);
    BinaryFile<TestRecord1> file(bytes);
    int index = 0;
    for (auto &record : file)
    {
        QCOMPARE(record, detail::constData1[index]);
        ++index;
    }
}

void GenTestClass::binaryFileConstIterTest()
{
    using namespace detail;
    using namespace Files;
    auto bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData1[0]), arrayDataSize);
    const BinaryFile<TestRecord1> file(bytes);
    int index = 0;
    for (const auto &record : file)
    {
        QCOMPARE(record, detail::constData1[index]);
        ++index;
    }
}

void GenTestClass::binaryFileReverseIterTest()
{
    using namespace detail;
    using namespace Files;
    auto bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData1[0]), arrayDataSize);
    BinaryFile<TestRecord1> file(bytes);
    int index = constDataSize - 1;
    for (auto iter = file.rbegin(); iter != file.rend(); ++iter)
    {
        QCOMPARE(*iter, detail::constData1[index]);
        --index;
    }
}

void GenTestClass::binaryFileFirstLastTest()
{
    using namespace detail;
    using namespace Files;
    auto bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData1[0]), arrayDataSize);
    BinaryFile<TestRecord1> file(bytes);
    const BinaryFile<TestRecord1> constFile(bytes);

    auto &first(file.first());
    auto &constFirst(constFile.first());
    QCOMPARE(first, detail::constData1[0]);
    QCOMPARE(constFirst, detail::constData1[0]);
    QCOMPARE(first, constFirst);

    constexpr auto lastIndex = constDataSize - 1;
    auto &last(file.last());
    auto &constLast(constFile.last());
    QCOMPARE(last, detail::constData1[lastIndex]);
    QCOMPARE(constLast, detail::constData1[lastIndex]);
    QCOMPARE(last, constLast);
}

void GenTestClass::binaryFileCopyAsTest()
{
    using namespace detail;
    using namespace Files;
    auto bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData1[0]), arrayDataSize);
    const BinaryFile<TestRecord1> constFile(bytes);
    auto file(constFile.copyAs<TestRecord2>());
    const auto &constFirst(constFile.first());
    auto &first(file.first());
    quint64 value = constFirst.value_b;
    value = (value << 32) | constFirst.value_a;
    QCOMPARE(value, first.value_a);
}

void GenTestClass::binaryFileFindRangeTest()
{
    using namespace detail;
    using namespace Files;
    auto bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData2[0]), arrayDataSize);
    BinaryFile<TestRecord1> file(bytes);
    const BinaryFile<TestRecord1> constFile(bytes);

    auto firstRange { decltype(file)::findRange(file.begin(), file.end(), testPredicate) };
    std::ptrdiff_t firstSize = firstRange.end - firstRange.begin;
    QVERIFY(firstRange.begin != file.begin());
    QVERIFY(firstRange.end != file.end());
    QCOMPARE(firstSize, 3);
    int index = 6;
    for (auto iter = firstRange.begin; iter != firstRange.end; ++iter, ++index)
    {
        QVERIFY(testPredicate(*iter));
        QCOMPARE(*iter, constData2[index]);
    }

    auto secondRange { file.findRange(testPredicate) };
    std::ptrdiff_t secondSize = secondRange.end - secondRange.begin;
    QVERIFY(secondRange.begin != file.begin());
    QVERIFY(secondRange.end != file.end());
    QCOMPARE(secondSize, 3);
    for (auto first = firstRange.begin, second = secondRange.begin; //
         first != firstRange.end && second != secondRange.end;      //
         ++first, ++second)                                         //
    {
        QCOMPARE(*first, *second);
    }

    auto thirdRange { BinaryFile<TestRecord1>::findRange(constFile.begin(), constFile.end(), testPredicate) };
    std::ptrdiff_t thirdSize = thirdRange.end - thirdRange.begin;
    QVERIFY(thirdRange.begin != constFile.begin());
    QVERIFY(thirdRange.end != constFile.end());
    QCOMPARE(thirdSize, 3);
    index = 6;
    for (auto iter = thirdRange.begin; iter != thirdRange.end; ++iter, ++index)
    {
        QVERIFY(testPredicate(*iter));
        QCOMPARE(*iter, constData2[index]);
    }

    auto fourthRange { constFile.findRange(testPredicate) };
    std::ptrdiff_t fourthSize = fourthRange.end - fourthRange.begin;
    QVERIFY(fourthRange.begin != constFile.begin());
    QVERIFY(fourthRange.end != constFile.end());
    QCOMPARE(fourthSize, 3);
    for (auto third = thirdRange.begin, fourth = fourthRange.begin; //
         third != thirdRange.end && fourth != fourthRange.end;      //
         ++third, ++fourth)                                         //
    {
        QCOMPARE(*third, *fourth);
    }
}

void GenTestClass::binaryFileFindReverseRangeTest()
{
    using namespace detail;
    using namespace Files;
    auto bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData2[0]), arrayDataSize);
    BinaryFile<TestRecord1> file(bytes);
    auto range { decltype(file)::findRange(file.rbegin(), file.rend(), testPredicate) };
    int index = 8;
    for (auto iter = range.begin; iter != range.end; ++iter, --index)
    {
        QVERIFY(testPredicate(*iter));
        QCOMPARE(*iter, constData2[index]);
    }
}

void GenTestClass::binaryFileFindAllRangesTest()
{
    using namespace detail;
    using namespace Files;
    auto bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData3[0]), arrayDataSize);
    BinaryFile<TestRecord1> file(bytes);
    const BinaryFile<TestRecord1> constFile(bytes);

    auto ranges { file.findAllRanges(testPredicate) };
    QCOMPARE(ranges.size(), 3);
    int index = 0;
    for (auto &range : ranges)
    {
        QVERIFY(!(range.begin == file.begin() && range.end == file.end()));
        for (auto iter = range.begin; iter != range.end; ++iter, ++index)
        {
            QVERIFY(testPredicate(*iter));
            QCOMPARE(*iter, constData3[index]);
        }
        index += 2;
    }

    auto constRanges { constFile.findAllRanges(testPredicate) };
    QCOMPARE(constRanges.size(), 3);
    index = 0;
    for (auto &range : constRanges)
    {
        QVERIFY(!(range.begin == constFile.begin() && range.end == constFile.end()));
        for (auto iter = range.begin; iter != range.end; ++iter, ++index)
        {
            QVERIFY(testPredicate(*iter));
            QCOMPARE(*iter, constData3[index]);
        }
        index += 2;
    }
}

void GenTestClass::binaryFileRemoveTest()
{
    using namespace detail;
    using namespace Files;
    auto bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData1[0]), arrayDataSize);
    BinaryFile<TestRecord1> file(bytes);
    QCOMPARE(file.size(), constDataSize);
    file.remove(file.begin(), file.begin() + 4);
    QCOMPARE(file.size(), constDataSize - 4);
    QCOMPARE(*file.begin(), constData1[4]);
}
