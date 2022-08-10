#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED 1

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <QtGlobal>
#include <QtAlgorithms>
#include <QScopedArrayPointer>

#define SORTING_NETWORK_MAX_INPUT 128

#define QSTR(str)  QStringLiteral(str)
#define CSTR(str)  QLatin1String(str)

typedef std::integral_constant<int, 0> zero_type;
using std::false_type;
using std::size_t;

template<typename T, int N>
inline constexpr int array_size(T (&)[N]) { return N; }

template<typename INT>
inline INT ceil_log_2(INT x) {
    typedef typename std::make_unsigned<INT>::type UNSIGNED;
    auto shift = sizeof(UNSIGNED) * 8 - qCountLeadingZeroBits(static_cast<UNSIGNED>(x) - 1);
    return (x == 0) ? 0 : shift;
}

template<typename INT>
inline INT highest_bit(INT x) {
    typedef typename std::make_unsigned<INT>::type UNSIGNED;
    auto shift = sizeof(UNSIGNED) * 8 - 1 - qCountLeadingZeroBits(static_cast<UNSIGNED>(x));
    return 1 << shift;
}

template<typename T>
class IntegralArray : public QScopedArrayPointer<T, QScopedPointerPodDeleter> {
    using Base = QScopedArrayPointer<T, QScopedPointerPodDeleter>;
    inline IntegralArray(void* ptr) : Base(static_cast<T*>(ptr)) {
        Q_CHECK_PTR(this->data());
    }
public:
    inline void fill(std::size_t n, std::integral_constant<T, 0>) { 
        std::memset(this->data(), 0, n * sizeof(T));
    }
    inline IntegralArray(std::size_t n) : IntegralArray(std::malloc(n * sizeof(T))) {}
    inline IntegralArray(std::size_t n, std::integral_constant<T, 0>) : IntegralArray(std::calloc(n, sizeof(T))) {}
    using Base::data;
    using Base::operator[];
};

#endif