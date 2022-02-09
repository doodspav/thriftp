#ifndef THRIFTP_CORE_MACROS_HPP
#define THRIFTP_CORE_MACROS_HPP


namespace thriftp::_detail {

    constexpr void noop_unlikely() noexcept { [[unlikely]] return; }

}  // namespace thriftp::_detail


// THRIFTP_UNREACHABLE() -> void

#if defined(_MSC_VER)
    #define THRIFTP_UNREACHABLE() __assume(0)
#elif defined(__GNUC__)
    #define THRIFTP_UNREACHABLE() __builtin_unreachable()
#else
    #define THRIFTP_UNREACHABLE() ::thriftp::_detail::noop_unlikely()
#endif


#endif  // THRIFTP_CORE_MACROS_HPP
