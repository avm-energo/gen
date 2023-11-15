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
    void initTestCase()
    {
    }

    // вызывается перед каждой тестовой функцией
    void init()
    {
    }

    // вызывается после каждой тестовой функции
    void cleanup()
    {
    }

    // вызывается после последней тестовой функции
    void cleanupTestCase()
    {
    }

    void binaryFileCtorSizeTest();
    void binaryFileIterTest();
    void binaryFileConstIterTest();
    void binaryFileFirstLastTest();
    void binaryFileCopyAsTest();
};
