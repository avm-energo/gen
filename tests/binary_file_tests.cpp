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
    { 1, 2, 3, 4 },     // 0
    { 5, 6, 7, 8 },     // 1
    { 9, 10, 11, 12 },  // 2
    { 13, 14, 15, 16 }, // 3
    { 17, 18, 19, 20 }, // 4
    { 21, 22, 23, 24 }, // 5
    { 25, 26, 27, 28 }, // 6
    { 29, 30, 31, 32 }, // 7
    { 33, 34, 35, 36 }, // 8
    { 37, 38, 39, 40 }, // 9
    { 41, 42, 43, 44 }, // 10
    { 45, 46, 47, 48 }  // 11
};

static const TestRecord1 constData2[constDataSize] {
    { 1, 2, 3, 4 },        // 0
    { 5, 6, 7, 8 },        // 1
    { 9, 10, 11, 12 },     // 2
    { 13, 14, 15, 16 },    // 3
    { 17, 18, 19, 20 },    // 4
    { 21, 22, 23, 24 },    // 5
    { maxValue, 0, 1, 2 }, // 6
    { 3, maxValue, 4, 5 }, // 7
    { 6, 7, maxValue, 8 }, // 8
    { 37, 38, 39, 40 },    // 9
    { 41, 42, 43, 44 },    // 10
    { 45, 46, 47, 48 }     // 11
};

static const TestRecord1 constData3[constDataSize] {
    { 1, 2, maxValue, 4 },    // 0
    { 5, maxValue, 7, 8 },    // 1
    { 9, 10, 11, 12 },        // 2
    { 13, 14, 15, 16 },       // 3
    { 17, maxValue, 19, 20 }, // 4
    { maxValue, 22, 23, 24 }, // 5
    { maxValue, 0, 1, 2 },    // 6
    { maxValue, 3, 4, 5 },    // 7
    { 6, 7, 7, 8 },           // 8
    { 37, 38, 39, 40 },       // 9
    { maxValue, 42, 43, 44 }, // 10
    { maxValue, 46, 47, 48 }  // 11
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
    QVERIFY(firstRange);
    QVERIFY(firstRange.begin != file.end());
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
    QVERIFY(secondRange);
    QVERIFY(secondRange.begin != file.end());
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
    QVERIFY(thirdRange);
    QVERIFY(thirdRange.begin != constFile.end());
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
    QVERIFY(fourthRange);
    QVERIFY(fourthRange.begin != constFile.end());
    QVERIFY(fourthRange.end != constFile.end());
    QCOMPARE(fourthSize, 3);
    for (auto third = thirdRange.begin, fourth = fourthRange.begin; //
         third != thirdRange.end && fourth != fourthRange.end;      //
         ++third, ++fourth)                                         //
    {
        QCOMPARE(*third, *fourth);
    }

    bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData1[0]), arrayDataSize);
    BinaryFile<TestRecord1> fileWithoutPattern(bytes);
    auto fifthRange { fileWithoutPattern.findRange(testPredicate) };
    QVERIFY(fifthRange.begin == fileWithoutPattern.end());
    QVERIFY(fifthRange.end == fileWithoutPattern.end());
    QVERIFY(!fifthRange);
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

    // Removing elements from the start
    file.remove(file.begin(), file.begin() + 4);
    auto size = file.size();
    QCOMPARE(size, constDataSize - 4);
    QCOMPARE(*file.begin(), constData1[4]);
    QCOMPARE(*(file.end() - 1), constData1[11]);

    // Incorrect removing
    file.remove(file.begin(), file.begin());
    file.remove(file.end(), file.end());
    file.remove(file.end(), file.end() + 5);
    QCOMPARE(size, file.size());
    QCOMPARE(*file.begin(), constData1[4]);
    QCOMPARE(*(file.end() - 1), constData1[11]);

    // Removing elements from the end
    file.remove(file.end() - 4, file.end());
    QCOMPARE(file.size(), constDataSize - 8);
    QCOMPARE(*file.begin(), constData1[4]);
    QCOMPARE(*(file.end() - 1), constData1[7]);

    // Removing elements from the mid
    file = BinaryFile<TestRecord1>(bytes);
    QCOMPARE(file.size(), constDataSize);
    file.remove(file.begin() + 2, file.end() - 2);
    QCOMPARE(file.size(), 4);
    QCOMPARE(*file.begin(), constData1[0]);
    QCOMPARE(*(file.begin() + 1), constData1[1]);
    QCOMPARE(*(file.end() - 2), constData1[10]);
    QCOMPARE(*(file.end() - 1), constData1[11]);
}

void GenTestClass::binaryFileMoveFromStartToEndTest()
{
    // Moving elements from the start to the end
    using namespace detail;
    using namespace Files;
    auto bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData1[0]), arrayDataSize);
    BinaryFile<TestRecord1> file(bytes);
    auto rangeSize = 6;
    auto result = file.move(file.begin(), file.begin() + rangeSize, file.end());
    auto fileSize = file.size();
    QVERIFY(result);
    QCOMPARE(file.last(), constData1[rangeSize - 1]);
    QCOMPARE(file.size(), fileSize);
    int index = 0;
    for (auto iter = file.end() - 1; iter != file.end() - 1 - rangeSize; --iter, ++index)
        QCOMPARE(*iter, constData1[rangeSize - (1 + index)]);
}

void GenTestClass::binaryFileMoveFromStartToMidTest()
{
    // Moving elements from the start to the mid
    using namespace detail;
    using namespace Files;
    auto bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData1[0]), arrayDataSize);
    BinaryFile<TestRecord1> file(bytes);
    auto rangeSize = 2;
    auto result = file.move(file.begin(), file.begin() + rangeSize, file.begin() + 4);
    QVERIFY(result);
    QCOMPARE(*file.begin(), constData1[2]);
    QCOMPARE(*(file.begin() + 1), constData1[3]);
    QCOMPARE(*(file.begin() + 2), constData1[0]);
    QCOMPARE(*(file.begin() + 3), constData1[1]);
}

void GenTestClass::binaryFileMoveFromEndToStartTest()
{
    // Moving elements from the end to the start
    using namespace detail;
    using namespace Files;
    auto bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData1[0]), arrayDataSize);
    BinaryFile<TestRecord1> file(bytes);
    auto rangeSize = 4, index = constDataSize - rangeSize;
    auto result = file.move(file.end() - rangeSize, file.end(), file.begin());
    QVERIFY(result);
    for (auto iter = file.begin(); iter != (file.begin() + rangeSize); ++iter, ++index)
        QCOMPARE(*iter, constData1[index]);
}

void GenTestClass::binaryFileMoveFromEndToMidTest()
{
    // Moving elements from the end to the mid
    using namespace detail;
    using namespace Files;
    auto bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData1[0]), arrayDataSize);
    BinaryFile<TestRecord1> file(bytes);
    int rangeSize = 3, pos = 4, index = constDataSize - rangeSize;
    auto result = file.move(file.end() - rangeSize, file.end(), file.begin() + pos);
    QVERIFY(result);
    for (auto iter = file.begin() + pos; iter != (file.begin() + pos + rangeSize); ++iter, ++index)
        QCOMPARE(*iter, constData1[index]);
}

void GenTestClass::binaryFileMoveFromMidToStartTest()
{
    // Moving elements from the mid to the start
    using namespace detail;
    using namespace Files;
    auto bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData1[0]), arrayDataSize);
    BinaryFile<TestRecord1> file(bytes);
    int rangeSize = 5, pos = 5, index = pos;
    auto result = file.move(file.begin() + pos, file.begin() + pos + rangeSize, file.begin());
    QVERIFY(result);
    for (auto iter = file.begin(); iter != file.begin() + rangeSize; ++iter, ++index)
        QCOMPARE(*iter, constData1[index]);
}

void GenTestClass::binaryFileMoveFromMidToEndTest()
{
    // Moving elements from the mid to the end
    using namespace detail;
    using namespace Files;
    auto bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData1[0]), arrayDataSize);
    BinaryFile<TestRecord1> file(bytes);
    int rangeSize = 6, pos = 3, index = pos;
    auto result = file.move(file.begin() + pos, file.begin() + pos + rangeSize, file.end());
    QVERIFY(result);
    for (auto iter = file.end() - rangeSize; iter != file.end(); ++iter, ++index)
        QCOMPARE(*iter, constData1[index]);
}

/*******************************************************
 *        An Explanation for Future Generations        *
 *         Given BinaryFile with 12 elements:          *
 * | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | *
 *                 -------------       ^               *
 *                       |             |               *
 *                       --------------|               *
 *   Moving 3 elements from the source index 4 to the  *
 *   destination index 9 gives next expected result:   *
 * | 0 | 1 | 2 | 3 | 7 | 8 | 4 | 5 | 6 | 9 | 10 | 11 | *
 *                         -------------               *
 *******************************************************/
void GenTestClass::binaryFileMoveFromMidToPosTest()
{
    // Moving elements from the mid to the another pos
    using namespace detail;
    using namespace Files;
    auto bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData1[0]), arrayDataSize);
    BinaryFile<TestRecord1> file(bytes);
    const int rangeSize = 3, pos = 4, destPos = pos + rangeSize + 2;
    auto result = file.move(file.begin() + pos, file.begin() + pos + rangeSize, file.begin() + destPos);
    QVERIFY(result);
    QCOMPARE(file.first(), constData1[0]);
    QCOMPARE(*(file.begin() + 4), constData1[7]);
    QCOMPARE(*(file.begin() + 5), constData1[8]);
    QCOMPARE(*(file.begin() + 6), constData1[4]);
    QCOMPARE(*(file.begin() + 7), constData1[5]);
    QCOMPARE(*(file.begin() + 8), constData1[6]);
    QCOMPARE(file.last(), constData1[constDataSize - 1]);
}
