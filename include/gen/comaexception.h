#pragma once

#include "gen_export.h"

#include <QException>
#include <QMap>
#include <QObject>

class GENLIB_EXPORT ComaException : public QException
{

public:
    ComaException(const QString &msg);
    ComaException();

    void raise() const override
    {
        throw *this;
    }
    ComaException *clone() const override
    {
        return new ComaException(*this);
    }

    QString message();

private:
    QString m_message;
};
