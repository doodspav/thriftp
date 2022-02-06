#ifndef THRIFTP_CORE_EXCEPTIONS_HPP
#define THRIFTP_CORE_EXCEPTIONS_HPP


#include <concepts>
#include <exception>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <thriftp/core/traits.hpp>
#include <thriftp/core/types.hpp>


namespace thriftp {


    class exception
        : public std::exception
    {
    public:
        exception()
            : m_what("thriftp::exception: (default)")
        {}

        exception(std::string what)
            : m_what(std::move(what))
        {}

        [[nodiscard]] const char*
        what() const noexcept override
        {
            return m_what.c_str();
        }

    protected:
        std::string m_what;
    };


    template <class S>
    class exception_with_state
        : public virtual exception
    {
    public:
        template <class T>
            requires std::constructible_from<S, T>
        exception_with_state(T&& state)
            : exception(""),
              m_state(std::forward<T>(state))
        {
            m_what += "thriftp::exception_with_state<S>: (default)";
            m_what += " [with S = ";
            m_what += type_name_v<S>;
            m_what += "]";
        }

        template <class T>
            requires std::constructible_from<S, T>
        exception_with_state(T&& state, std::string what)
             : exception(std::move(what)),
               m_state(std::forward<T>(state))
        {}

        [[nodiscard]] S&
        state() noexcept
        {
            return m_state;
        }

        [[nodiscard]] const S&
        state() const noexcept
        {
            return m_state;
        }

    protected:
        S m_state;
    };


    template <class T, class... Args>
    exception_with_state(T&&, Args&&...)
        -> exception_with_state<std::decay_t<T>>;


    class application_exception
        : public virtual exception
    {
    public:
        enum class error_type
        {
            UNKNOWN,
            INVALID_PROTOCOL,
            INVALID_MESSAGE_TYPE,
            UNKNOWN_METHOD,
            WRONG_METHOD_NAME,
            BAD_SEQUENCE_ID,
            MISSING_RESULT,
            // helpers
            _min = UNKNOWN,
            _max = MISSING_RESULT
        };

        [[nodiscard]] static constexpr std::string_view
        error_type_sv(error_type type) noexcept
        {
            switch (type)
            {
                case error_type::UNKNOWN:              return "UNKNOWN";
                case error_type::INVALID_PROTOCOL:     return "INVALID_PROTOCOL";
                case error_type::INVALID_MESSAGE_TYPE: return "INVALID_MESSAGE_TYPE";
                case error_type::UNKNOWN_METHOD:       return "UNKNOWN_METHOD";
                case error_type::WRONG_METHOD_NAME:    return "WRONG_METHOD_NAME";
                case error_type::BAD_SEQUENCE_ID:      return "BAD_SEQUENCE_ID";
                case error_type::MISSING_RESULT:       return "MISSING_RESULT";
                default: return "(invalid error_type)";
            }
        }

        application_exception()
            : application_exception(error_type::UNKNOWN)
        {}

        application_exception(error_type type)
            : exception(""),
              m_type(type)
        {
            m_what += "thriftp::application_exception: ";
            m_what += error_type_sv(m_type);
        }

        application_exception(std::string what)
            : application_exception(error_type::UNKNOWN, std::move(what))
        {}

        application_exception(error_type type, std::string what)
            : exception(std::move(what)),
              m_type(type)
        {}

        [[nodiscard]] error_type
        type() const noexcept
        {
            return m_type;
        }

    protected:
        error_type m_type;
    };


    template <class S>
    class application_exception_with_state
        : public application_exception,
          public exception_with_state<S>
    {
    public:
        template <class T>
            requires std::constructible_from<S, T>
        application_exception_with_state(T&& state)
            : application_exception_with_state(std::forward<T>(state),
                                               error_type::UNKNOWN)
        {}

        template <class T>
            requires std::constructible_from<S, T>
        application_exception_with_state(T&& state, error_type type)
            : exception(""),
              application_exception(type, ""),
              exception_with_state<S>(std::forward<T>(state), "")
        {
            m_what += "thriftp::application_exception_with_state<S>: ";
            m_what += error_type_sv(m_type);
            m_what += " [with S = ";
            m_what += type_name_v<S>;
            m_what += "]";
        }

        template <class T>
            requires std::constructible_from<S, T>
        application_exception_with_state(T&& state, std::string what)
            : application_exception_with_state(std::forward<T>(state),
                                               error_type::UNKNOWN,
                                               std::move(what))
        {}

        template <class T>
            requires std::constructible_from<S, T>
        application_exception_with_state(T&& state, error_type type, std::string what)
            : exception(std::move(what)),
              application_exception(type, ""),
              exception_with_state<S>(std::forward<T>(state), "")
        {}
    };


    template <class T, class... Args>
    application_exception_with_state(T&&, Args&&...)
        -> application_exception_with_state<std::decay_t<T>>;


    class protocol_exception
        : public virtual exception
    {
    public:
        enum class error_type
        {
            UNKNOWN,
            BAD_VERSION,
            INVALID_DATA,
            WRONG_FIELD_TYPE,
            OUT_OF_RANGE,
            NEGATIVE_SIZE,
            SIZE_LIMIT,
            DEPTH_LIMIT,
            MISSING_REQUIRED_FIELD,
            // helpers
            _min = UNKNOWN,
            _max = MISSING_REQUIRED_FIELD
        };

        [[nodiscard]] static constexpr std::string_view
        error_type_sv(error_type type) noexcept
        {
            switch (type)
            {
                case error_type::UNKNOWN:                return "UNKNOWN";
                case error_type::BAD_VERSION:            return "BAD_VERSION";
                case error_type::INVALID_DATA:           return "INVALID_DATA";
                case error_type::WRONG_FIELD_TYPE:       return "WRONG_FIELD_TYPE";
                case error_type::OUT_OF_RANGE:           return "OUT_OF_RANGE";
                case error_type::NEGATIVE_SIZE:          return "NEGATIVE_SIZE";
                case error_type::SIZE_LIMIT:             return "SIZE_LIMIT";
                case error_type::DEPTH_LIMIT:            return "DEPTH_LIMIT";
                case error_type::MISSING_REQUIRED_FIELD: return "MISSING_REQUIRED_FIELD";
                default: return "(invalid error_type)";
            }
        }

        protocol_exception(fid_t field_id)
            : protocol_exception(field_id, error_type::UNKNOWN)
        {}

        protocol_exception(fid_t field_id, error_type type)
            : exception(""),
              m_type(type),
              m_field_id(field_id)
        {
            m_what += "thriftp::protocol_exception (field_id=";
            m_what += std::to_string(m_field_id);
            m_what += "): ";
            m_what += error_type_sv(m_type);
        }

        protocol_exception(fid_t field_id, std::string what)
            : protocol_exception(field_id, error_type::UNKNOWN, std::move(what))
        {}

        protocol_exception(fid_t field_id, error_type type, std::string what)
            : exception(std::move(what)),
              m_type(type),
              m_field_id(field_id)
        {}

        [[nodiscard]] error_type
        type() const noexcept
        {
            return m_type;
        }

        [[nodiscard]] fid_t
        field_id() const noexcept
        {
            return m_field_id;
        }

    protected:
        error_type m_type;
        fid_t m_field_id;
    };


    template <class S>
    class protocol_exception_with_state
        : public protocol_exception,
          public exception_with_state<S>
    {
    public:
        template <class T>
            requires std::constructible_from<S, T>
        protocol_exception_with_state(T&& state, fid_t field_id)
            : protocol_exception_with_state(std::forward<T>(state), field_id,
                                            error_type::UNKNOWN)
        {}

        template <class T>
            requires std::constructible_from<S, T>
        protocol_exception_with_state(T&& state, fid_t field_id, error_type type)
            : exception(""),
              protocol_exception(field_id, type, ""),
              exception_with_state<S>(std::forward<T>(state), "")
        {
            m_what += "thriftp::protocol_exception_with_state<S> (field_id=";
            m_what += std::to_string(m_field_id);
            m_what += "): ";
            m_what += error_type_sv(m_type);
            m_what += " [with S = ";
            m_what += type_name_v<S>;
            m_what += "]";
        }

        template <class T>
            requires std::constructible_from<S, T>
        protocol_exception_with_state(T&& state, fid_t field_id, std::string what)
            : protocol_exception_with_state(std::forward<T>(state), field_id,
                                            error_type::UNKNOWN, std::move(what))
        {}

        template <class T>
            requires std::constructible_from<S, T>
        protocol_exception_with_state(T&& state, fid_t field_id, error_type type, std::string what)
            : exception(std::move(what)),
              protocol_exception(field_id, type, ""),
              exception_with_state<S>(std::forward<T>(state), "")
        {}
    };


    template <class T, class... Args>
    protocol_exception_with_state(T&&, Args&&...)
        -> protocol_exception_with_state<std::decay_t<T>>;


    class io_exception
        : public virtual exception
    {
    public:
        enum class error_type
        {
            UNKNOWN,
            INPUT_EOF,
            OUTPUT_EOF,
            // helpers
            _min = UNKNOWN,
            _max = OUTPUT_EOF
        };

        [[nodiscard]] static constexpr std::string_view
        error_type_sv(error_type type) noexcept
        {
            switch (type)
            {
                case error_type::UNKNOWN:    return "UNKNOWN";
                case error_type::INPUT_EOF:  return "INPUT_EOF";
                case error_type::OUTPUT_EOF: return "OUTPUT_EOF";
                default: return "(invalid error_type)";
            }
        }

        io_exception(size_t remaining)
            : io_exception(remaining, error_type::UNKNOWN)
        {}

        io_exception(size_t remaining, error_type type)
            : exception(""),
              m_type(type),
              m_remaining(remaining)
        {
            m_what += "thriftp::io_exception (remaining=";
            m_what += std::to_string(m_remaining);
            m_what += "): ";
            m_what += error_type_sv(m_type);
        }

        io_exception(size_t remaining, std::string what)
            : io_exception(remaining, error_type::UNKNOWN, std::move(what))
        {}

        io_exception(size_t remaining, error_type type, std::string what)
            : exception(std::move(what)),
              m_type(type),
              m_remaining(remaining)
        {}

        [[nodiscard]] error_type
        type() const noexcept
        {
            return m_type;
        }

        [[nodiscard]] size_t
        remaining() const noexcept
        {
            return m_remaining;
        }

    protected:
        error_type m_type;
        size_t m_remaining;
    };


    template <class S>
    class io_exception_with_state
        : public io_exception,
          public exception_with_state<S>
    {
    public:
        template <class T>
            requires std::constructible_from<S, T>
        io_exception_with_state(T&& state, size_t remaining)
            : io_exception_with_state(std::forward<T>(state), remaining,
                                      error_type::UNKNOWN)
        {}

        template <class T>
            requires std::constructible_from<S, T>
        io_exception_with_state(T&& state, size_t remaining, error_type type)
            : exception(""),
              io_exception(remaining, type, ""),
              exception_with_state<S>(std::forward<T>(state), "")
        {
            m_what += "thriftp::io_exception_with_state<S> (remaining=";
            m_what += std::to_string(m_remaining);
            m_what += "): ";
            m_what += error_type_sv(m_type);
            m_what += " [with S = ";
            m_what += type_name_v<S>;
            m_what += "]";
        }

        template <class T>
            requires std::constructible_from<S, T>
        io_exception_with_state(T&& state, size_t remaining, std::string what)
            : io_exception_with_state(std::forward<T>(state), remaining,
                                      error_type::UNKNOWN, std::move(what))
        {}

        template <class T>
            requires std::constructible_from<S, T>
        io_exception_with_state(T&& state, size_t remaining, error_type type, std::string what)
            : exception(std::move(what)),
              io_exception(remaining, type, ""),
              exception_with_state<S>(std::forward<T>(state), "")
        {}
    };


    template <class T, class... Args>
    io_exception_with_state(T&&, Args&&...)
        -> io_exception_with_state<std::decay_t<T>>;


}  // namespace thriftp


#endif  // THRIFTP_CORE_EXCEPTIONS_HPP
