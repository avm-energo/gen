#pragma once

#include <QObject>
#include <QString>
#include <cmath>
#include <functional>
#include <gen/gen_export.h>
#include <memory>
#include <type_traits>

#define MAINSLEEP 20
#define MAINTIMEOUT 2000 // 2 sec timeout for general operations

/// \brief Namespace with internal helper structure
namespace QtHelper
{
/*! \brief Functor for auto-deleting
 *  \details Functor for auto-deleting object, used in UniquePointer as default deletor
 */
struct deleteLaterDeletor
{
    void operator()(QObject *object) const
    {
        if (object)
        {
            object->deleteLater();
        }
    }
};
}

template <typename T> using SharedPointer = std::shared_ptr<T>;

template <typename T> using UniquePointer = std::unique_ptr<T, QtHelper::deleteLaterDeletor>;

/// \brief Декларация для определния простых (POD) типов.
/// \see https://en.cppreference.com/w/cpp/types/is_pod
template <typename T> //
constexpr static auto is_simple_v = std::is_standard_layout_v<T> &&std::is_trivial_v<T>;

constexpr int defaultRatio = 3;
constexpr int maxRatio = 5;
constexpr auto resourceDirectory = ":/module";

/*! \brief Class contains most frequently used general purpose functions
 *  \details That class contains static general purpose functions
 */
class GENLIB_EXPORT StdFunc
{
private:
    static QString HomeDir;        ///< \private Рабочий каталог программы
    static QString SystemHomeDir;  ///< \private Системный каталог программы
    static int m_tuneRequestCount; ///< \private Степень усреднения для регулировки

public:
    static QString DeviceIP;             ///< Device's IP address
    static QString s_OrganizationString; ///< Name of organization
    static struct
    {
        bool cancelled = false;
        bool cancelEnabled = true;
    } state;

    StdFunc() = default;

    static void Init();
    static QString VerToStr(quint32);
    static quint32 StrToVer(const QString &str);
    static bool FloatIsWithinLimits(double var, double base, double tolerance);
    static float ToFloat(const QString &text, bool *ok = nullptr);
    static void SetHomeDir(const QString &dir);
    static QString GetHomeDir();
    static QString GetSystemHomeDir();
    static void SetDeviceIP(const QString &ip);
    static QString ForDeviceIP();
    static void SetOrganizationString(const QString &str);
    static QString OrganizationString();
    static void SetTuneRequestCount(int n);
    static int TuneRequestCount();
    static void Cancel();
    static void ClearCancel();
    static bool IsCancelled();
    static void SetCancelDisabled();
    static void SetCancelEnabled();
    static int IndexByBit(quint32 dword);
    static quint32 BitByIndex(int idx);
    static void Wait(int ms = MAINSLEEP);

    static inline int goldenRatio(int value)
    {
        int multiplier = value / 10;
        for (auto i = maxRatio + multiplier; i != defaultRatio; --i)
        {
            if (!(value % i))
                return i;
        }
        return defaultRatio + multiplier;
    }

    static quint32 Ping(quint32 addr);
    static quint32 CheckPort(quint32 ip4Addr, quint16 port);

    static void RemoveSubstr(std::string &str, std::string &substr);

    /*! \brief Template function for joining items into QList
     *  \param list[out] QList for joining input item
     *  \param item[in] Input item
     */
    template <typename T> static void joinItem(QList<T> &list, const T item)
    {
        list.append(item);
    }

    /// \brief Converts an instance of trivial data type to the byte array.
    /// \param value[in] Input data for convertation.
    template <typename T, std::size_t size = sizeof(T), std::enable_if_t<is_simple_v<T>, bool> = true> //
    static QByteArray toByteArray(const T &value)
    {
        return QByteArray::fromRawData(reinterpret_cast<const char *>(&value), size);
    }

    /// \brief Converts list of known datatype to QVariant list.
    template <typename T> static QVariantList ToVariantList(const QList<T> &list)
    {
        QVariantList newList;
        for (const T &item : list)
            newList.push_back(item);
        return newList;
    }

    /// \brief Converts QVariant list to list of pointers of known datatype.
    template <typename T> static QList<T> ToValuesList(const QList<T *> &list)
    {
        QList<T> newList;
        newList.reserve(list.size());
        std::transform(std::begin(list), std::end(list), std::back_inserter(newList), [](T *item) { return *item; });
        return newList;
    }

    /// \brief Returns count of bit set in input data.
    template <typename T, size_t size = sizeof(T), typename = std::enable_if<is_simple_v<T>, bool>> //
    static int CountSetBits(T N)
    {
        int count = 0;
        for (int i = 0; i < sizeof(T) * 8; i++)
        {
            if (N & (1 << i))
                count++;
        }
        return count;
    }
};
