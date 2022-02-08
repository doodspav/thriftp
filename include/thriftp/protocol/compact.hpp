#ifndef THRIFTP_PROTOCOL_COMPACT_HPP
#define THRIFTP_PROTOCOL_COMPACT_HPP


#include <cstddef>
#include <iterator>
#include <limits>
#include <utility>

#include <thriftp/core/concepts.hpp>
#include <thriftp/core/exceptions.hpp>
#include <thriftp/core/traits.hpp>
#include <thriftp/core/types.hpp>

#include <thriftp/protocol/_detail/io.hpp>
#include <thriftp/protocol/_detail/sstack.hpp>


namespace thriftp::protocol {


    namespace _detail {


        struct compact_base
        {
        public:
            static constexpr unsigned char PROTOCOL_ID  = 0x82u;
            static constexpr unsigned char VERSION_N    = 1u;
            static constexpr unsigned char VERSION_MASK = 0b0001'1111u;
            static constexpr unsigned char TYPE_MAX     = 0b1110'0000u;
            static constexpr unsigned char TYPE_SHIFT_AMOUNT = 5u;

            enum class type : unsigned char
            {
                STOP = 0,
                BOOL = 1, TRUE = 1,
                FALSE = 2,
                BYTE = 3,
                I16 = 4,
                I32 = 5,
                I64 = 6,
                DOUBLE = 7,
                STRING = 8, BINARY = 8,
                LIST = 9,
                SET = 10,
                MAP = 11,
                STRUCT = 12,
                // helpers
                _min = STOP,
                _max = STRUCT
            };

        protected:
            using aet = application_exception::error_type;
            using pet = protocol_exception::error_type;
            using iet = io_exception::error_type;
        };


        template <std::size_t N, UCharInputIterator I, std::sentinel_for<I> S>
            requires (N != std::numeric_limits<decltype(N)>::max())
        class compact_reader
            : private reader_base<I, S>,
              private compact_base
        {
        private:
            using reader_type = reader_base<I, S>;

            static_stack<fid_t, N + 1> m_fields;

        public:
            using typename reader_type::iterator_type;
            using reader_type::move_iterator;

            constexpr compact_reader(I&& it, const S& sen, size_constant<N> = {})
                noexcept(std::is_nothrow_constructible_v<reader_type, I, const S&>)
                : reader_type(std::move(it), sen)
            {
                // make sure that stack is never empty
                // so that on error we don't have to check if it is
                m_fields.push(0);
            }

            template <Byte T>
            constexpr T
            read_byte()
            {
                unsigned char val;
                if (!reader_type::read_one(val)) [[unlikely]]
                {
                    throw io_exception_with_state(
                        move_iterator(),
                        1,  // bytes not read
                        iet::INPUT_EOF,
                        "read_byte() failed to read 1/1 bytes"
                    );
                }

                auto byte = static_cast<T>(val);
                if constexpr (CHAR_BIT != 8)
                {
                    if (!bits<8>::in_range(byte)) [[unlikely]]
                    {
                        throw protocol_exception_with_state(
                            move_iterator(),
                            m_fields.top(),
                            pet::INVALID_DATA,
                            "read_byte() value exceeds 8 bits"
                        );
                    }
                }

                return byte;
            }

        };


        template <std::size_t N, UCharOutputIterator O, std::sentinel_for<O> S>
            requires (N != std::numeric_limits<decltype(N)>::max())
        class compact_writer
            : private writer_base<O, S>,
              private compact_base
        {
        private:
            using writer_type = writer_base<O, S>;

            static_stack<fid_t, N + 1> m_fields;

        public:
            using typename writer_type::iterator_type;
            using writer_type::move_iterator;

            constexpr compact_writer(O&& ot, const S& sen, size_constant<N> = {})
                noexcept(std::is_nothrow_constructible_v<writer_type, O, const S&>)
                : writer_type(std::move(ot), sen)
            {
                // make sure that stack is never empty
                // so that on error we don't have to check if it is
                m_fields.push(0);
            }

            constexpr void
            write_byte(Byte auto byte)
            {
                if (!bits<8>::in_range(byte))
                {
                    throw protocol_exception_with_state(
                        move_iterator(),
                        m_fields.top(),
                        pet::OUT_OF_RANGE,
                        "write_byte() value exceeds 8 bits"
                    );
                }

                auto val = static_cast<unsigned char>(byte);
                if (!writer_type::write_one(val)) [[unlikely]]
                {
                    throw io_exception_with_state(
                        move_iterator(),
                        1,  // bytes not written
                        iet::OUTPUT_EOF,
                        "write_byte() failed to write 1/1 bytes"
                    );
                }
            }
        };


    }  // namespace _detail


    struct compact;


}  // namespace thriftp::protocol


#endif  // THRIFTP_PROTOCOL_COMPACT_HPP

