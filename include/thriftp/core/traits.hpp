#ifndef THRIFTP_CORE_TRAITS_HPP
#define THRIFTP_CORE_TRAITS_HPP


#include <cstddef>
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


}  // namespace thriftp


#endif  // THRIFTP_CORE_TRAITS_HPP
