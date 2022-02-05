#ifndef THRIFTP_PROTOCOL_DETAIL_ZIGZAG_HPP
#define THRIFTP_PROTOCOL_DETAIL_ZIGZAG_HPP


#include <limits>
#include <type_traits>

#include <thriftp/core/concepts.hpp>


namespace thriftp::protocol::_detail {


    struct zigzag
    {
        [[nodiscard]] constexpr static auto
        encode(thriftp::_detail::IntAnyN<2> auto n) noexcept
        {
            using int_t = decltype(n);
            using uint_t = std::make_unsigned_t<int_t>;

            constexpr unsigned digits = std::numeric_limits<int_t>::digits;

            auto a = static_cast<uint_t>(n >> digits);
            auto b = static_cast<uint_t>(static_cast<uint_t>(n) << 1u);

            return static_cast<uint_t>(a ^ b);
        }

        [[nodiscard]] constexpr static auto
        decode(thriftp::_detail::UIntAnyN<2> auto n) noexcept
        {
            using uint_t = decltype(n);
            using int_t = std::make_signed_t<uint_t>;

            auto a = static_cast<uint_t>(n >> 1u);
            auto b = static_cast<uint_t>(-static_cast<int_t>(n & 1u));

            return static_cast<int_t>(a ^ b);
        }
    };


}  // namespace thriftp::protocol::_detail


#endif  // THRIFTP_PROTOCOL_DETAIL_ZIGZAG_HPP
