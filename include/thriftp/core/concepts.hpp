#ifndef THRIFTP_CORE_CONCEPTS_HPP
#define THRIFTP_CORE_CONCEPTS_HPP


#include <climits>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <limits>
#include <ranges>
#include <type_traits>
#include <utility>


// generic helpers

namespace thriftp::_detail {


    template <class T>
    concept Mutable =
        !std::is_const_v<std::remove_reference_t<T>>;


    template <class From, class To>
    concept ExplicitlyConvertibleTo =
    requires {
        static_cast<To>(std::declval<From>());
    };


}  // namespace thriftp::_detail


// arithmetic types

namespace thriftp {


    template <class T>
    concept Bool =
        std::same_as<std::remove_cv_t<T>, bool>;


    namespace _detail {


        template <class T>
        concept Integer =
            std::integral<T> &&
            !Bool<T>;


        template <class T, std::size_t N>
        concept IntAnyN =
            N >= 8 &&
            Integer<T> &&
            std::signed_integral<T> &&
            std::numeric_limits<T>::digits >= (N - 1);


        template <class T, std::size_t N>
        concept UIntAnyN =
            N >= 8 &&
            Integer<T> &&
            std::unsigned_integral<T> &&
            std::numeric_limits<T>::digits >= N;


        template <class T, std::size_t N>
        concept IntExactN =
            IntAnyN<T, N> &&
            std::numeric_limits<T>::digits == (N - 1);


        template <class T, std::size_t N>
        concept UIntExactN =
            UIntAnyN<T, N> &&
            std::numeric_limits<T>::digits == N;


    }  // namespace _detail


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
        _detail::UIntAnyN<T, std::numeric_limits<std::size_t>::digits>;


    template <class T>
    concept ISize =
        _detail::IntAnyN<T, std::numeric_limits<std::size_t>::digits>;


}  // namespace thriftp


// iterators and ranges

namespace thriftp {


    namespace _detail {


        template <class I, class T>
        concept InputIteratorAssignTo =
            std::input_iterator<I> &&
            std::assignable_from<T, std::iter_reference_t<I>>;


        template <class I, class T>
        concept InputIteratorCastTo =
            std::input_iterator<I> &&
            ExplicitlyConvertibleTo<std::iter_reference_t<I>, T>;


        template <class I, class T>
        concept OutputIteratorAssignFrom =
            std::output_iterator<I, T>;


        template <class I, class T>
        concept OutputIteratorCastFrom =
            requires {
                typename std::iter_value_t<I>;
            } &&
            std::output_iterator<I, std::iter_value_t<I>> &&
            ExplicitlyConvertibleTo<T, std::iter_value_t<I>>;


        template <class In, class Out>
        concept IteratorPassthroughPair =
            std::input_iterator<In> &&
            std::output_iterator<Out, std::iter_reference_t<In>>;


    }  // namespace _detail


    template <class I>
    concept UCharInputIterator =
        // if Assign works, Cast will always work, so just check for Cast
        _detail::InputIteratorCastTo<I, unsigned char>;


    template <class I>
    concept UCharOutputIterator =
        _detail::OutputIteratorAssignFrom<I, unsigned char> ||
        _detail::OutputIteratorCastFrom<I, unsigned char>;


    template <class R>
    concept UcharInputRange =
        std::ranges::range<R> &&
        UCharInputIterator<std::ranges::iterator_t<R>>;


    template <class R>
    concept UCharOutputRange =
        std::ranges::range<R> &&
        UCharOutputIterator<std::ranges::iterator_t<R>>;


}  // namespace thriftp


#endif  // THRIFTP_CORE_CONCEPTS_HPP
