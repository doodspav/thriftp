#ifndef THRIFTP_CORE_TYPES_HPP
#define THRIFTP_CORE_TYPES_HPP


#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <thriftp/core/concepts.hpp>


// ANY type aliased here may be changed as long as all the static assertions at
// the end of the file still pass

// NONE of the static assertions may be changed


// arithmetic types  (CAN BE MODIFIED)

namespace thriftp {


    using bool_t = bool;

    using byte_t = std::int16_t;
    using int16_t = std::int16_t;
    using int32_t = std::int32_t;
    using int64_t = std::int64_t;

    using double_t = double;

    using size_t = std::size_t;
    using ssize_t = std::common_type_t<std::ptrdiff_t, std::make_signed_t<size_t>>;

    using fid_t = std::int16_t;  // field id type


}  // namespace thriftp


// static assertions  (DO NOT CHANGE)

namespace thriftp {


    static_assert(Bool<bool_t>, "bad thriftp::bool_t type");

    static_assert(Byte<byte_t>, "unsupported thriftp::byte_t type");
    static_assert(Int16<int16_t>, "unsupported thriftp::int16_t type");
    static_assert(Int32<int32_t>, "unsupported thriftp::int32_t type");
    static_assert(Int64<int64_t>, "unsupported thriftp::int64_t type");

    static_assert(Double<double_t>, "unsupported thriftp::double_t type");

    static_assert(USize<size_t>, "unsupported thriftp::size_t type");
    static_assert(ISize<ssize_t>, "unsupported thriftp::ssize_t type");

    static_assert(Int16<fid_t>, "unsupported thriftp::fid_t type");


}  // namespace thriftp


#endif  // THRIFTP_CORE_TYPES_HPP
