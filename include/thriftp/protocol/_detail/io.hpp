#ifndef THRIFTP_PROTOCOL_DETAIL_IO_HPP
#define THRIFTP_PROTOCOL_DETAIL_IO_HPP


#include <iterator>

#include <thriftp/core/concepts.hpp>
#include <thriftp/core/types.hpp>


namespace thriftp::protocol::_detail {


    template <class I, class O>
    concept UCharPassthrough =
        UCharInputIterator<I> &&
        UCharOutputIterator<O> &&
        thriftp::_detail::IteratorPassthroughPair<I, O>;


    template <class I, class O>
    concept UCharCastIn =
        !UCharPassthrough<I, O> &&
        thriftp::_detail::InputIteratorCastTo<I, unsigned char> &&
        thriftp::_detail::OutputIteratorAssignFrom<O, unsigned char>;


    template <class I, class O>
    concept UCharCastInOut =
        !UCharPassthrough<I, O> &&
        !UCharCastIn<I, O> &&
        thriftp::_detail::InputIteratorCastTo<I, unsigned char> &&
        thriftp::_detail::OutputIteratorCastFrom<O, unsigned char>;


    struct io_base
    {
        template <class I, class O>
        static constexpr void
        copy_advance_nocheck(I& it, O& ot)
            noexcept(noexcept(*ot = *it) && noexcept(++ot) && noexcept(++it))
            requires UCharPassthrough<I, O>
        {
            *ot = *it;
            ++ot;
            ++it;
        }

        template <class I, class O>
        static constexpr void
        copy_advance_nocheck(I& it, O& ot)
            noexcept(noexcept(*ot = static_cast<unsigned char>(*it)) &&
                     noexcept(++ot) && noexcept(++it))
            requires UCharCastIn<I, O>
        {
            *ot = static_cast<unsigned char>(*it);
            ++ot;
            ++it;
        }

        template <class I, class O>
        static constexpr void
        copy_advance_nocheck(I& it, O& ot)
            noexcept(noexcept(*ot = static_cast<std::iter_value_t<O>>(
                              static_cast<unsigned char>(*it))) &&
                     noexcept(++ot) && noexcept(++it))
            requires UCharCastInOut<I, O>
        {
            using V = std::iter_value_t<O>;
            *ot = static_cast<V>(static_cast<unsigned char>(*it));
            ++ot;
            ++it;
        }
    };


    struct read_base
        : private io_base
    {
        template <UCharInputIterator I>
        static constexpr void
        read_one_nocheck(I& it, unsigned char& val)
            noexcept(noexcept(static_cast<unsigned char>(*it)) &&
                     noexcept(++it))
        {
            val = static_cast<unsigned char>(*it);
            ++it;
        }

        template <UCharInputIterator I, std::sentinel_for<I> S>
        [[nodiscard]] static constexpr bool
        read_one(I& it, const S& sen, unsigned char& val)
            noexcept(noexcept(static_cast<bool>(it == sen)) &&
                     noexcept(read_one_nocheck(it, val)))
        {
            if (it == sen) [[unlikely]] { return false; }
            else [[likely]] { read_one_nocheck(it, val); return true; }
        }

        template <UCharInputIterator I, UCharOutputIterator O>
        static constexpr void
        read_n_nocheck(I& it, O& ot, std::size_t n)
            noexcept(noexcept(copy_advance_nocheck(it, ot)))
        {
            while (n > 0) { copy_advance_nocheck(it, ot); --n; }
        }

        template <UCharInputIterator I, std::sentinel_for<I> S, UCharOutputIterator O>
        [[nodiscard]] static constexpr std::size_t
        read_n(I& it, const S& sen, O& ot, std::size_t n)
            noexcept(noexcept(static_cast<bool>(it == sen)) &&
                     noexcept(copy_advance_nocheck(it, ot)))
        {
            while (n > 0)
            {
                if (it == sen) [[unlikely]] { return n; }
                else [[likely]] { copy_advance_nocheck(it, ot); --n; }
            }
            return 0;
        }

        template <UCharInputIterator I, std::sized_sentinel_for<I> S, UCharOutputIterator O>
        [[nodiscard]] static constexpr std::size_t
        read_n(I& it, const S& sen, O& ot, std::size_t n)
            noexcept(noexcept(read_n_nocheck(it, ot, n)) &&
                     noexcept(std::iter_difference_t<I>(sen - it)))
            requires thriftp::_detail::Integer<std::iter_difference_t<I>>

        {
            using diff_t = std::iter_difference_t<I>;  // likely signed
            auto idist = static_cast<diff_t>(sen - it);
            auto udist = static_cast<std::make_unsigned_t<diff_t>>(idist);
            // double check because dist is (likely) signed and n is unsigned
            if (idist < 0 || udist < n) [[unlikely]] { return n; }
            else [[likely]] { read_n_nocheck(it, ot, n); return 0; }
        }
    };


    struct write_base
        : private io_base
    {
        template <UCharOutputIterator O>
        static constexpr void
        write_one_nocheck(O& ot, unsigned char val)
            noexcept(noexcept(copy_advance_nocheck(
                              std::declval<const unsigned char*&>(), ot)))
        {
            const unsigned char *it = &val;
            copy_advance_nocheck(it, ot);
        }

        template <UCharOutputIterator O, std::sentinel_for<O> S>
        [[nodiscard]] static constexpr bool
        write_one(O& ot, const S& sen, unsigned char val)
            noexcept(noexcept(static_cast<bool>(ot == sen)) &&
                     noexcept(write_one_nocheck(ot, val)))
        {
            if (ot == sen) [[unlikely]] { return false; }
            else [[likely]] { write_one_nocheck(ot, val); return true; }
        }

        template <UCharOutputIterator O, UCharInputIterator I>
        static constexpr void
        write_n_nocheck(O& ot, I& it, std::size_t n)
            noexcept(noexcept(copy_advance_nocheck(it, ot)))
        {
            while (n > 0) { copy_advance_nocheck(it, ot); --n; }
        }

        template <UCharOutputIterator O, std::sentinel_for<O> S, UCharInputIterator I>
        [[nodiscard]] static constexpr std::size_t
        write_n(O& ot, const S& sen, I& it, std::size_t n)
            noexcept(noexcept(static_cast<bool>(ot == sen)) &&
                     noexcept(copy_advance_nocheck(it, ot)))
        {
            while (n > 0)
            {
                if (ot == sen) [[unlikely]] { return n; }
                else [[likely]] { copy_advance_nocheck(it, ot); --n; }
            }
            return 0;
        }

        template <UCharOutputIterator O, std::sized_sentinel_for<O> S, UCharInputIterator I>
        [[nodiscard]] static constexpr std::size_t
        write_n(O& ot, const S& sen, I& it, std::size_t n)
            noexcept(noexcept(write_n_nocheck(ot, it, n)) &&
                     noexcept(std::iter_difference_t<O>(sen - ot)))
            requires thriftp::_detail::Integer<std::iter_difference_t<O>>
        {
            using diff_t = std::iter_difference_t<O>;
            auto idist = static_cast<diff_t>(sen - ot);
            auto udist = static_cast<std::make_unsigned_t<diff_t>>(idist);
            // double check because dist is (likely) signed and n is unsigned
            if (idist < 0 || udist < n) [[unlikely]] { return n; }
            else [[likely]] { write_n_nocheck(ot, it, n); return 0; }
        }
    };


}  // namespace thriftp::protocol::_detail


#endif  // THRIFTP_PROTOCOL_DETAIL_IO_HPP
