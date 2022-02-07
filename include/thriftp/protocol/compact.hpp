#ifndef THRIFTP_PROTOCOL_COMPACT_HPP
#define THRIFTP_PROTOCOL_COMPACT_HPP


#include <thriftp/core/exceptions.hpp>


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


    }  // namespace _detail


    struct compact;


}  // namespace thriftp::protocol


#endif  // THRIFTP_PROTOCOL_COMPACT_HPP

