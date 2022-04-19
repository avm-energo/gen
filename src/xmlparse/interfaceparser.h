#ifndef BASEINTERFACE_H
#define BASEINTERFACE_H
#include "../datatypes.h"
#include "../settings/interfacesettings.h"
#include "../settings/settings.h"

#include <memory>
#include <typeinfo>

class InterfaceParser
{

public:
    template <class Group> static InterfaceSettings parseSettings(QDomElement domElement)
    {
        using Register = typename InterfaceInfo<Group>::Register;
#ifdef XML_DEBUG
        qDebug() << domElement.text();
        qDebug() << "TagName: " << domElement.tagName();
#endif
        const auto &nodes = domElement.childNodes();
        // NOTE Temporary commented
        // Q_ASSERT(!nodes.isEmpty());
        int i = 0;
        InterfaceInfo<Group> settings;

        while (i != nodes.count())
        {
            const auto &domElement = nodes.item(i++).toElement();
            if (domElement.tagName().contains("group", Qt::CaseInsensitive))
            {
                Group group(domElement);
                settings.addGroup(group);
            }
            else if (domElement.tagName().contains("register", Qt::CaseInsensitive))
            {
                Register reg(domElement);
                settings.addReg(reg);
            }
#ifdef XML_DEBUG
            qDebug() << group.attribute("id", "") << group.text();
#endif
        }
#ifdef XML_DEBUG
        qDebug() << settings.groups().count();
#endif
        return InterfaceSettings { QVariant::fromValue(settings) };
    }
    virtual InterfaceSettings parseSettings(QDomElement domElement) const = 0;
};

#endif // BASEINTERFACE_H
