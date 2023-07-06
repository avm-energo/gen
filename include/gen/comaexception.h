#pragma once

#include <QException>
#include <QMap>
#include <QObject>
#include <gen/gen_export.h>

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
