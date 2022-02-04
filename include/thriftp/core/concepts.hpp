#ifndef THRIFTP_CORE_CONCEPTS_HPP
#define THRIFTP_CORE_CONCEPTS_HPP


#include <climits>
#include <concepts>
#include <cstddef>
#include <limits>


// arithmetic types

namespace thriftp {


    namespace _detail {


        template <class T, std::size_t N>
        concept IntAnyN =
            N != 0 &&
            std::signed_integral<T> &&
            std::numeric_limits<T>::digits >= (N - 1);


        template <class T, std::size_t N>
        concept UIntAnyN =
            N != 0 &&
            std::unsigned_integral<T> &&
            std::numeric_limits<T>::digits >= N;


    }  // namespace _detail


    template <class T>
    concept Bool =
        std::same_as<T, bool>;


    template <class T>
    concept Byte =
        _detail::IntAnyN<T, 8>;


    template <class T>
    concept Int16 =
        _detail::IntAnyN<T, 16>;


    template <class T>
    concept Int32 =
        _detail::IntAnyN<T, 32>;


    template <class T>
    concept Int64 =
        _detail::IntAnyN<T, 64>;


    template <class T>
    concept Double =
        std::floating_point<T> &&
        std::numeric_limits<T>::is_iec559 &&
        std::numeric_limits<T>::digits == 53 &&
        sizeof(T) == 8 &&
        CHAR_BIT * sizeof(T) == 64;


    template <class T>
    concept USize =
        _detail::UIntAnyN<T, std::numeric_limits<T>::digits>;


    template <class T>
    concept ISize =
        _detail::IntAnyN<T, std::numeric_limits<T>::digits>;


}  // namespace thriftp


#endif  // THRIFTP_CORE_CONCEPTS_HPP
