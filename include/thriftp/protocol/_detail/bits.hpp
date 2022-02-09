#ifndef THRIFTP_PROTOCOL_DETAIL_BITS_HPP
#define THRIFTP_PROTOCOL_DETAIL_BITS_HPP


#include <limits>
#include <type_traits>

#include <thriftp/core/concepts.hpp>
#include <thriftp/core/traits.hpp>


namespace thriftp::protocol::_detail {


    template <unsigned N>
        requires (N != 0 && N <= std::numeric_limits<unsigned long long>::digits)
    struct bits
    {
        using signed_type = fast_signed_t<N>;
        using unsigned_type = fast_unsigned_t<N>;

        [[nodiscard]] static consteval unsigned_type
        umax() noexcept
        {
            unsigned_type n = 0;
            for (auto i = 0u; i < N; ++i)
            {
                n <<= 1u;
                n |= 1u;
            }
            return n;
        }

        [[nodiscard]] static consteval signed_type
        imax() noexcept
        {
            signed_type n = 0;
            for (auto i = 0u; i < (N - 1u); ++i)
            {
                n <<= 1;
                n |= 1;
            }
            return n;
        }

        [[nodiscard]] static consteval signed_type
        imin() noexcept
        {
            return static_cast<signed_type>(-imax() - 1);
        }

        [[nodiscard]] static consteval unsigned_type
        vsize() noexcept
        {
            auto s = static_cast<unsigned_type>(N / 7u);
            s += ((N % 7u) ? 1u : 0u);
            return s;
        }

        [[nodiscard]] static consteval unsigned_type
        vlast() noexcept
        {
            unsigned_type n = 0;
            for (auto i = 0u; i < (N % 7u); ++i)
            {
                n <<= 1u;
                n |= 1u;
            }
            return n;
        }

        [[nodiscard]] static consteval unsigned_type
        vshift() noexcept
        {
            return static_cast<unsigned_type>(N - (N % 7u));
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

        [[nodiscard]] static constexpr bool
        in_range(thriftp::_detail::IntAnyN<N> auto iN) noexcept
        {
            return iN >= imin() && iN <= imax();
        }

        [[nodiscard]] static constexpr bool
        in_range(thriftp::_detail::IntExactN<N> auto iN) noexcept
        {
            return true;
        }

        [[nodiscard]] static constexpr bool
        in_range(thriftp::_detail::UIntAnyN<N> auto uN) noexcept
        {
            return uN <= umax();
        }

        [[nodiscard]] static constexpr bool
        in_range(thriftp::_detail::UIntExactN<N> auto uN) noexcept
        {
            return true;
        }
    };


}  // namespace thriftp::protocol::_detail


#endif  // THRIFTP_PROTOCOL_DETAIL_BITS_HPP
