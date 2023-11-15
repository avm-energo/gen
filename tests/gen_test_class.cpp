#include "gen_test_class.h"

#include <array>
#include <gen/datatypes.h>
#include <gen/error.h>
#include <gen/stdfunc.h>

GenTestClass::GenTestClass(QObject *parent) : QObject(parent)
{
}

void GenTestClass::binaryFileTest()
{
    ;
}

QTEST_GUILESS_MAIN(GenTestClass)
