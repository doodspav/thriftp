#ifndef THRIFTP_PROTOCOL_DETAIL_SSTACK_HPP
#define THRIFTP_PROTOCOL_DETAIL_SSTACK_HPP


#include <array>
#include <cassert>
#include <cstddef>
#include <type_traits>


namespace thriftp::protocol::_detail {


    template <class T, std::size_t N>
        requires std::is_trivial_v<T>
    class static_stack
    {
    public:
        using value_type      = T;
        using size_type       = std::size_t;
        using reference       = T&;
        using const_reference = const T&;

    private:
        size_type m_size = 0;
        std::array<T, N> m_arr;

    public:
        [[nodiscard]] constexpr bool
        empty() const noexcept
        {
            return m_size == 0;
        }

        [[nodiscard]] constexpr size_type
        size() const noexcept
        {
            return m_size;
        }

        [[nodiscard]] constexpr size_type
        capacity() const noexcept
        {
            return N;
        }

        [[nodiscard]] constexpr size_type
        max_size() const noexcept
        {
            return N;
        }

        [[nodiscard]] constexpr reference
        top() noexcept
        {
            assert(!empty());
            return m_arr[m_size - 1];
        }

        [[nodiscard]] constexpr const_reference
        top() const noexcept
        {
            assert(!empty());
            return m_arr[m_size - 1];
        }

        constexpr void
        push(value_type value) noexcept(std::is_nothrow_assignable_v<T&, T>)
        {
            assert(size() < capacity());
            m_arr[m_size++] = value;
        }

        constexpr void
        pop() noexcept
        {
            assert(!empty());
            --m_size;
        }

        constexpr void
        clear() noexcept
        {
            m_size = 0;
        }

        constexpr void
        swap(static_stack& other) noexcept
        {
            std::swap(m_size, other.m_size);
            std::swap(m_arr, other.m_arr);
        }
    };


}  // namespace thriftp::protocol::_detail


#endif  // THRIFTP_PROTOCOL_DETAIL_SSTACK_HPP
