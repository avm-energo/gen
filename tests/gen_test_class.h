#pragma once

#include <QObject>
#include <QtTest>

class GenTestClass : public QObject
{
    Q_OBJECT
public:
    explicit GenTestClass(QObject *parent = nullptr);

private slots:
    // вызывается перед первой тестовой функцией
    void initTestCase();

    // вызывается перед каждой тестовой функцией
    void init();

    // вызывается после каждой тестовой функции
    void cleanup();

    // вызывается после последней тестовой функции
    void cleanupTestCase();

    void binaryFileCtorSizeTest();
    void binaryFileIterTest();
    void binaryFileConstIterTest();
    void binaryFileFirstLastTest();
    void binaryFileCopyAsTest();
    void binaryFileFindRangeTest();
    void binaryFileFindAllRangesTest();
    void binaryFileRemoveTest();
    void binaryFileMoveFromStartToEndTest();
    void binaryFileMoveFromStartToMidTest();
    void binaryFileMoveFromEndToStartTest();
    void binaryFileMoveFromEndToMidTest();
    void binaryFileMoveFromMidToStartTest();
    void binaryFileMoveFromMidToEndTest();
    void binaryFileMoveFromMidToPosTest();

    void verToStrTest();
    void strToVerTest();
    void enumToStrTest();

    void stdToByteArrayTest01();
    void stdToByteArrayTest02();
    void stdToByteArrayTest03();
    void stdToByteArrayTest04();

    void stdSafeMemoryCopyTest01();
    void stdSafeMemoryCopyTest02();
    void stdGetFromByteArrayTest01();
    void stdGetFromByteArrayTest02();
};
