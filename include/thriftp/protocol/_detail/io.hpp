#ifndef THRIFTP_PROTOCOL_DETAIL_IO_HPP
#define THRIFTP_PROTOCOL_DETAIL_IO_HPP


#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

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


    template <UCharInputIterator I, std::sentinel_for<I> S>
    class reader_base
        : private io_base
    {
    public:
        using iterator_type = I;

        [[nodiscard]] constexpr iterator_type&&
        move_iterator() noexcept
        {
            return std::move(m_it);
        }

        constexpr reader_base(I&& it, const S& sen)
            noexcept(std::is_nothrow_move_constructible_v<I> &&
                     std::is_nothrow_copy_constructible_v<S>)
            : m_it(std::move(it)),
              m_sen(sen)
        {}

        constexpr void
        read_one_nocheck(unsigned char& val)
            noexcept(noexcept(static_cast<unsigned char>(*m_it)) &&
                     noexcept(++m_it))
        {
            val = static_cast<unsigned char>(*m_it);
            ++m_it;
        }

        [[nodiscard]] constexpr bool
        read_one(unsigned char& val)
            noexcept(noexcept(static_cast<bool>(m_it == m_sen)) &&
                     noexcept(read_one_nocheck(val)))
        {
            if (m_it == m_sen) [[unlikely]] { return false; }
            else [[likely]] { read_one_nocheck(val); return true; }
        }

        constexpr void
        read_n_nocheck(UCharOutputIterator auto& ot, std::size_t n)
            noexcept(noexcept(copy_advance_nocheck(m_it, ot)))
        {
            while (n > 0) { copy_advance_nocheck(m_it, ot); --n; }
        }

        [[nodiscard]] constexpr std::size_t
        read_n(UCharOutputIterator auto& ot, std::size_t n)
            noexcept(noexcept(static_cast<bool>(m_it == m_sen)) &&
                     noexcept(copy_advance_nocheck(m_it, ot)))
        {
            while (n > 0)
            {
                if (m_it == m_sen) [[unlikely]] { return n; }
                else [[likely]] { copy_advance_nocheck(m_it, ot); --n; }
            }
            return 0;
        }

        [[nodiscard]] constexpr std::size_t
        read_n(UCharOutputIterator auto& ot, std::size_t n)
            noexcept(noexcept(read_n_nocheck(ot, n)) &&
                     noexcept(std::iter_difference_t<I>(m_sen - m_it)))
            requires std::sized_sentinel_for<S, I> &&
                     thriftp::_detail::Integer<std::iter_difference_t<I>>
        {
            using diff_t = std::iter_difference_t<I>;  // likely signed
            auto idist = static_cast<diff_t>(m_sen - m_it);
            auto udist = static_cast<std::make_unsigned_t<diff_t>>(idist);

            // double check because idist is (likely) signed and n is unsigned
            if (idist < 0 || udist < n) [[unlikely]] { return n; }
            else [[likely]] { read_n_nocheck(ot, n); return 0; }
        }

    private:
        [[no_unique_address]] I m_it;
        [[no_unique_address]] S m_sen;
    };


    template <UCharOutputIterator O, std::sentinel_for<O> S>
    class writer_base
        : private io_base
    {
    public:
        using iterator_type = O;

        [[nodiscard]] constexpr iterator_type&&
        move_iterator() noexcept
        {
            return std::move(m_ot);
        }

        constexpr writer_base(O&& ot, const S& sen)
            noexcept(std::is_nothrow_move_constructible_v<O> &&
                     std::is_nothrow_copy_constructible_v<S>)
            : m_ot(std::move(ot)),
              m_sen(sen)
        {}

        constexpr void
        write_one_nocheck(unsigned char val)
            noexcept(noexcept(copy_advance_nocheck(
                              std::declval<const unsigned char*&>(), m_ot)))
        {
            const unsigned char *it = &val;
            copy_advance_nocheck(it, m_ot);
        }

        [[nodiscard]] constexpr bool
        write_one(unsigned char val)
            noexcept(noexcept(static_cast<bool>(m_ot == m_sen)) &&
                     noexcept(write_one_nocheck(val)))
        {
            if (m_ot == m_sen) [[unlikely]] { return false; }
            else [[likely]] { write_one_nocheck(val); return true; }
        }

        constexpr void
        write_n_nocheck(UCharInputIterator auto& it, std::size_t n)
            noexcept(noexcept(copy_advance_nocheck(it, m_ot)))
        {
            while (n > 0) { copy_advance_nocheck(it, m_ot); --n; }
        }

        [[nodiscard]] constexpr std::size_t
        write_n(UCharInputIterator auto& it, std::size_t n)
            noexcept(noexcept(static_cast<bool>(m_ot == m_sen)) &&
                     noexcept(copy_advance_nocheck(it, m_ot)))
        {
            while (n > 0)
            {
                if (m_ot == m_sen) [[unlikely]] { return n; }
                else [[likely]] { copy_advance_nocheck(it, m_ot); --n; }
            }
            return 0;
        }

        [[nodiscard]] constexpr std::size_t
        write_n(UCharInputIterator auto& it, std::size_t n)
            noexcept(noexcept(write_n_nocheck(it, n)) &&
                     noexcept(std::iter_difference_t<O>(m_sen - m_ot)))
            requires std::sized_sentinel_for<S, O> &&
                     thriftp::_detail::Integer<std::iter_difference_t<O>>
        {
            using diff_t = std::iter_difference_t<O>;  // likely signed
            auto idist = static_cast<diff_t>(m_sen - m_ot);
            auto udist = static_cast<std::make_unsigned_t<diff_t>>(idist);

            // double check because idist is (likely) signed and n is unsigned
            if (idist < 0 || udist < n) [[unlikely]] { return n; }
            else [[likely]] { write_n_nocheck(it, n); return 0; }
        }

    private:
        [[no_unique_address]] O m_ot;
        [[no_unique_address]] S m_sen;
    };


}  // namespace thriftp::protocol::_detail


#endif  // THRIFTP_PROTOCOL_DETAIL_IO_HPP
