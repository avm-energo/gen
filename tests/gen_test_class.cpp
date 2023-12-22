#include "gen_test_class.h"

GenTestClass::GenTestClass(QObject *parent) : QObject(parent)
{
}

void GenTestClass::initTestCase()
{
}

void GenTestClass::init()
{
}

void GenTestClass::cleanup()
{
}

void GenTestClass::cleanupTestCase()
{
}

QTEST_GUILESS_MAIN(GenTestClass)
