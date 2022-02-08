#ifndef THRIFTP_CORE_TRAITS_HPP
#define THRIFTP_CORE_TRAITS_HPP


#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include <type_traits>


namespace thriftp {


    namespace _detail {


        template <class T>
        [[nodiscard]] consteval auto
#if defined(_MSC_VER)
        __cdecl
#endif
        type_name()
        {
            std::string_view ns = "thriftp::_detail::";
            std::string_view name, prefix, suffix;

#if defined(__clang__)
            name = __PRETTY_FUNCTION__ ;
            prefix = "auto type_name() [T = ";
            suffix = "]";
#elif defined(__GNUC__)
            name = __PRETTY_FUNCTION__;
            prefix = "consteval auto type_name() [with T = ";
            suffix = "]";
#elif defined(_MSC_VER)
            name = __FUNCSIG__;
            prefix = "auto __cdecl type_name<";
            suffix = ">(void)";
#endif

            if (name.size() > (ns.size() + prefix.size() + suffix.size()))
            {
                name.remove_prefix(ns.size() + prefix.size());
                name.remove_suffix(suffix.size());
            }
            else { name = "(type_name not supported on this compiler)"; }

            return name;
        }


        template <unsigned N>
            requires (N != 0 && N <= std::numeric_limits<unsigned long long>::digits)
        [[nodiscard]] consteval std::integral auto
        exact_or_fast_signed_zero() noexcept
        {
            if constexpr (N < 8) { return std::int_fast8_t{}; }
            else if constexpr (N == 8)
            {
#ifdef INT8_MAX
                return std::int8_t{};
#else
                return std::int_fast8_t{};
#endif
            }
            else if constexpr (N < 16) { return std::int_fast16_t{}; }
            else if constexpr (N == 16)
            {
#ifdef INT16_MAX
                return std::int16_t{};
#else
                return std::int_fast16_t{};
#endif
            }
            else if constexpr (N < 32) { return std::int_fast32_t{}; }
            else if constexpr (N == 32)
            {
#ifdef INT32_MAX
                return std::int32_t{};
#else
                return std::int_fast32_t{};
#endif
            }
            else if constexpr (N < 64) { return std::int_fast64_t{}; }
            else if constexpr (N == 64)
            {
#ifdef INT64_MAX
                return std::int64_t{};
#else
                return std::int_fast64_t{};
#endif
            }
            // explicit cast for clarity
            else { return static_cast<long long>(0); }
        }


    }  // namespace _detail


    template <class T>
    struct type_name
    {
        static constexpr std::string_view value = _detail::type_name<T>();
    };

    template <class T>
    inline constexpr std::string_view type_name_v = type_name<T>::value;


    template <std::size_t N>
    using size_constant = std::integral_constant<std::size_t, N>;


    template <unsigned N>
    struct exact_or_fast_int
    {
        using type = decltype(_detail::exact_or_fast_signed_zero<N>());
    };

    template <unsigned N>
    using exact_or_fast_int_t = typename exact_or_fast_int<N>::type;


    template <unsigned N>
    struct exact_or_fast_uint
    {
        using type = std::make_signed_t<exact_or_fast_int_t<N>>;
    };

    template <unsigned N>
    using exact_or_fast_uint_t = typename exact_or_fast_uint<N>::type;


}  // namespace thriftp


#endif  // THRIFTP_CORE_TRAITS_HPP
