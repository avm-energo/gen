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
    );
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
    { 1, 2, 3, 4 }, { 5, 6, 7, 8 }, { 9, 10, 11, 12 },          //
    { 13, 14, 15, 16 }, { 17, 18, 19, 20 }, { 21, 22, 23, 24 }, //
    { maxValue, maxValue, maxValue, maxValue },                 //
    { maxValue, maxValue, maxValue, maxValue },                 //
    { maxValue, maxValue, maxValue, maxValue },                 //
    { 37, 38, 39, 40 }, { 41, 42, 43, 44 }, { 45, 46, 47, 48 }  //
};

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
    int iter = 0;
    for (auto &record : file)
    {
        QCOMPARE(record, detail::constData1[iter]);
        ++iter;
    }
}

void GenTestClass::binaryFileConstIterTest()
{
    using namespace detail;
    using namespace Files;
    auto bytes = QByteArray::fromRawData(reinterpret_cast<const char *>(&constData1[0]), arrayDataSize);
    const BinaryFile<TestRecord1> file(bytes);
    int iter = 0;
    for (const auto &record : file)
    {
        QCOMPARE(record, detail::constData1[iter]);
        ++iter;
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
    const BinaryFile<TestRecord1> file1(bytes);
    auto file2(file1.copyAs<TestRecord2>());
    const auto &first1(file1.first());
    auto &first2(file2.first());
}
