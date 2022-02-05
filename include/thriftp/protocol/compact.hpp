#ifndef THRIFTP_PROTOCOL_COMPACT_HPP
#define THRIFTP_PROTOCOL_COMPACT_HPP


#include <cstddef>
#include <iterator>
#include <limits>
#include <type_traits>

#include <thriftp/core/concepts.hpp>
#include <thriftp/core/exceptions.hpp>
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


        template <std::size_t N>
            requires (N != std::numeric_limits<decltype(N)>::max())
        class compact_read
            : private compact_base,
              private read_base
        {
        public:
            constexpr compact_read() noexcept
            {
                // make sure stack is never empty
                // so that on error we don't have to check if it is
                m_fields.push(0);
            }

        private:
            static_stack<fid_t, N + 1> m_fields;

        public:
            template <UCharInputIterator I, std::sentinel_for<I> S>
            constexpr void
            read_byte(I& it, const S& sen, Byte auto& byte)
                requires thriftp::_detail::Mutable<decltype(byte)>
            {
                unsigned char val;
                if (!read_one(it, sen, val)) [[unlikely]]
                {
                    throw io_exception_with_state(
                        std::move(it),
                        1,  // bytes not read
                        iet::INPUT_EOF,
                        "read_byte() failed to read 1/1 bytes"
                    );
                }
                byte = static_cast<std::decay_t<decltype(byte)>>(val);
            }
        };


        template <std::size_t N>
            requires (N != std::numeric_limits<decltype(N)>::max())
        class compact_write
            : private compact_base,
              private write_base
        {
        public:
            constexpr compact_write() noexcept
            {
                // make sure stack is never empty
                // so that on error we don't have to check if it is
                m_fields.push(0);
            }

        private:
            static_stack<fid_t, N + 1> m_fields;

        public:
            template <UCharOutputIterator O, std::sentinel_for<O> S>
            constexpr void
            write_byte(O& ot, const S& sen, Byte auto byte)
            {
                auto val = static_cast<unsigned char>(byte);
                if (!write_one(ot, sen, val)) [[unlikely]]
                {
                    throw io_exception_with_state(
                        std::move(ot),
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

