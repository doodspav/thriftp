#ifndef THRIFTP_PROTOCOL_DETAIL_BITS_HPP
#define THRIFTP_PROTOCOL_DETAIL_BITS_HPP


#include <limits>
#include <type_traits>

#include <thriftp/core/concepts.hpp>


namespace thriftp::protocol::_detail {


    template <unsigned N>
        requires (N != 0 && N <= std::numeric_limits<unsigned long long>::digits)
    struct bits
    {
        using int_t = signed long long;
        using uint_t = unsigned long long;

        [[nodiscard]] static consteval uint_t
        umax() noexcept
        {
            uint_t n = 0;
            for (auto i = 0u; i < N; ++i)
            {
                n <<= 1u;
                n |= 1u;
            }
            return n;
        }

        [[nodiscard]] static consteval int_t
        imax() noexcept
        {
            int_t n = 0;
            for (auto i = 0u; i < (N - 1u); ++i)
            {
                n <<= 1;
                n |= 1;
            }
            return n;
        }

        [[nodiscard]] static consteval int_t
        imin() noexcept
        {
            return static_cast<int_t>(-imax() - 1);
        }

        [[nodiscard]] static consteval uint_t
        vsize() noexcept
        {
            auto s = static_cast<uint_t>(N / 7u);
            s += ((N % 7u) ? 1u : 0u);
            return s;
        }

        [[nodiscard]] static consteval uint_t
        vlast() noexcept
        {
            uint_t n = 0;
            for (auto i = 0u; i < (N % 7u); ++i)
            {
                n <<= 1u;
                n |= 1u;
            }
            return n;
        }

        [[nodiscard]] static consteval uint_t
        vshift() noexcept
        {
            return static_cast<uint_t>(N - (N % 7));
        }

        [[nodiscard]] static constexpr auto
        to_signed(thriftp::_detail::UIntAnyN<N> auto uN) noexcept
        {
            using uN_t = decltype(uN);
            using iN_t = std::make_signed_t<uN_t>;

            // truncate to N bits
            uN &= static_cast<uN_t>(umax());

            // sign extension mask
            auto mask = static_cast<uN_t>(-1);
            mask <<= N;  // specialisation prevents UB
            mask *= bool(uN & static_cast<uN_t>(imin()));

            return static_cast<iN_t>(uN | mask);
        }

        [[nodiscard]] static constexpr auto
        to_signed(thriftp::_detail::UIntExactN<N> auto uN) noexcept
        {
            using uN_t = decltype(uN);
            using iN_t = std::make_signed_t<uN_t>;

            return static_cast<iN_t>(uN);
        }

        [[nodiscard]] static constexpr auto
        to_unsigned(thriftp::_detail::IntAnyN<N> auto iN) noexcept
        {
            using iN_t = decltype(iN);
            using uN_t = std::make_unsigned_t<iN_t>;

            // truncate to N bits
            auto uN = static_cast<uN_t>(iN);
            uN &= static_cast<uN_t>(umax());

            return uN;
        }

        [[nodiscard]] static constexpr auto
        to_unsigned(thriftp::_detail::IntExactN<N> auto iN) noexcept
        {
            using iN_t = decltype(iN);
            using uN_t = std::make_unsigned_t<iN_t>;

            return static_cast<uN_t>(iN);
        }
    };


}  // namespace thriftp::protocol::_detail


#endif  // THRIFTP_PROTOCOL_DETAIL_BITS_HPP
