#pragma once
#include <array>
#include <bitset>
#include <boost/pfr.hpp>
#include <format>
#include <iterator>
#include <magic_enum/magic_enum.hpp>
#include <optional>
#include <type_traits>
#include <variant>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class IPacket;

namespace formatter {

template <typename T>
    requires std::is_aggregate_v<T>
constexpr std::string toString(const T& value);

namespace detail {

template <typename T>
constexpr bool always_false_v = false;

template <typename T>
struct IsStdVariant : std::false_type {};

template <typename... Ts>
struct IsStdVariant<std::variant<Ts...>> : std::true_type {};

template <typename T>
struct IsStdArray : std::false_type {};

template <typename T, std::size_t N>
struct IsStdArray<std::array<T, N>> : std::true_type {};

template <typename T>
struct IsStdBitset : std::false_type {};

template <std::size_t N>
struct IsStdBitset<std::bitset<N>> : std::true_type {};

namespace concepts {

template <typename T>
concept IsOptional = requires { typename T::value_type; } && requires(T t) {
    t.has_value();
    t.value();
};

template <typename T>
concept IsVector = requires(T t) {
    typename T::value_type;
    t.begin();
    t.end();
};

template <typename T>
concept IsVariant = requires(T t) { typename std::remove_cvref_t<T>; } && IsStdVariant<std::remove_cvref_t<T>>::value;

template <typename T>
concept IsArray = IsStdArray<std::remove_cvref_t<T>>::value;

template <typename T>
concept IsMap = requires(T t) {
    typename T::key_type;
    typename T::mapped_type;
    t.begin();
    t.end();
};

template <typename T>
concept IsBitset = IsStdBitset<std::remove_cvref_t<T>>::value;

} // namespace concepts

template <typename T, typename F>
constexpr void forEachFieldWithName(T&& value, F&& func) {
    return boost::pfr::for_each_field(
        std::forward<T>(value),
        [f = std::forward<F>(func)](auto&& field, auto index) mutable {
            using IndexType     = decltype(index);
            using FieldType     = decltype(field);
            constexpr auto name = boost::pfr::detail::get_name<std::remove_reference_t<T>, IndexType::value>();
            bool           last = (index == boost::pfr::tuple_size_v<std::remove_reference_t<T>> - 1);
            f(name, std::forward<FieldType>(field), last);
        }
    );
}

constexpr bool isValidUTF8(std::string_view s) {
    auto       it  = s.begin();
    const auto end = s.end();

    while (it != end) {
        const std::uint8_t c         = static_cast<std::uint8_t>(*it++);
        std::size_t        remaining = 0;

        if (c <= 0x7F) {
            continue;
        } else if ((c & 0xE0) == 0xC0) {
            remaining = 1;
            if (c < 0xC2) return false;
        } else if ((c & 0xF0) == 0xE0) {
            remaining = 2;
        } else if ((c & 0xF8) == 0xF0) {
            remaining = 3;
            if (c > 0xF4) return false;
        } else {
            return false;
        }

        if (static_cast<std::size_t>(std::distance(it, end)) < remaining) return false;
        for (std::size_t i = 0; i < remaining; ++i, ++it) {
            if ((static_cast<std::uint8_t>(*it) & 0xC0) != 0x80) return false;
        }

        if (remaining == 1) {
            return true;
        } else if (remaining == 2) {
            const std::uint32_t cp = static_cast<std::uint32_t>((c & 0x0F) << 12)
                                   | static_cast<std::uint32_t>((static_cast<std::uint8_t>(*(it - 2)) & 0x3F) << 6)
                                   | static_cast<std::uint32_t>((static_cast<std::uint8_t>(*(it - 1)) & 0x3F));
            if (cp < 0x0800 || (cp >= 0xD800 && cp <= 0xDFFF)) return false;
        } else if (remaining == 3) {
            const std::uint32_t cp = static_cast<std::uint32_t>((c & 0x07) << 18)
                                   | static_cast<std::uint32_t>((static_cast<std::uint8_t>(*(it - 3)) & 0x3F) << 12)
                                   | static_cast<std::uint32_t>(((static_cast<std::uint8_t>(*(it - 2)) & 0x3F) << 6))
                                   | static_cast<std::uint32_t>((static_cast<std::uint8_t>(*(it - 1)) & 0x3F));
            if (cp < 0x10000 || cp > 0x10FFFF) return false;
        }
    }
    return true;
}

constexpr std::string dumpString(std::string_view content) {
    std::string result{};
    result.reserve(static_cast<size_t>(static_cast<double>(content.size()) * 1.05));

    for (char c : content) {
        switch (c) {
        case '"':
            result += "\\\"";
            break;
        case '\\':
            result += "\\\\";
            break;
        case '\b':
            result += "\\b";
            break;
        case '\f':
            result += "\\f";
            break;
        case '\n':
            result += "\\n";
            break;
        case '\r':
            result += "\\r";
            break;
        case '\t':
            result += "\\t";
            break;
        default:
            if (static_cast<uint8_t>(c) <= 0x1F) {
                result.append(std::format("\\u{:04x}", c));
            } else {
                result.push_back(c);
            }
        }
    }

    return result;
}

constexpr std::string formatString(std::string_view str) {
    if (isValidUTF8(str)) {
        return dumpString(str);
    } else {
        std::string out{"0x"};
        for (std::uint8_t c : str) {
            out.append(std::format("{:02X}", c));
        }
        return out;
    }
}

template <typename T>
constexpr std::string typeToString(const T& value) {
    if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
        return value.empty() ? "\"\"" : formatString(value);
    } else if constexpr (concepts::IsOptional<T>) {
        return value.has_value() ? typeToString(*value) : "null";
    } else if constexpr (concepts::IsArray<T>) {
        std::string out{"[ "};
        for (auto it = value.begin(); it != value.end(); ++it) {
            out.append(typeToString(*it));
            if (std::next(it) != value.end()) {
                out.append(", ");
            }
        }
        out.append(" ]");
        return out;
    } else if constexpr (concepts::IsMap<T>) {
        std::string out{"{"};
        for (auto it = value.begin(); it != value.end(); ++it) {
            out.append(std::format("{}: {}", typeToString(it->first), typeToString(it->second)));
            if (std::next(it) != value.end()) {
                out.append(", ");
            }
        }
        out.append(" }");
        return out;
    } else if constexpr (concepts::IsVector<T>) {
        std::string out{"[ "};
        for (auto it = value.begin(); it != value.end(); ++it) {
            out.append(typeToString(*it));
            if (std::next(it) != value.end()) {
                out.append(", ");
            }
        }
        out.append(" ]");
        return out;
    } else if constexpr (concepts::IsVariant<T>) {
        return std::visit([](const auto& v) { return typeToString(v); }, value);
    } else if constexpr (concepts::IsBitset<T>) {
        std::string out{"{ "};
        for (std::size_t i = 0; i < value.size(); ++i) {
            out.append(std::format("{}: {}", i, value.test(i)));
            if (i + 1 < value.size()) {
                out.append(", ");
            }
        }
        out.append(" }");
        return out;
    } else if constexpr (std::is_aggregate_v<T>) {
        return toString(value);
    } else if constexpr (std::is_enum_v<T>) {
        auto name            = magic_enum::enum_name(value);
        auto underlyingValue = static_cast<std::underlying_type_t<T>>(value);
        if (name.empty()) {
            return std::format("UnknownEnumValue<{}>", underlyingValue);
        }
        return std::format("{}({})", name, underlyingValue);
    } else if constexpr (std::formattable<T, char>) {
        return std::format("{}", value);
    } else {
        static_assert(always_false_v<T>, "Type cannot be converted to string");
    }
}

template <typename... Args, std::size_t... Is>
constexpr std::string formatPacketImpl(const IPacket& packet, std::index_sequence<Is...>, Args&&... args) {
    std::string out = std::format("{}({}) {{ ", packet.getName(), static_cast<int>(packet.getId()));
    ((out.append(args), (Is != sizeof...(Args) - 1 ? out.append(", ") : out)), ...);
    out.append(" }");
    return out;
}

} // namespace detail

template <typename T>
void append(std::string& out, std::string_view name, const T& value, bool last) {
    out.append(std::format("{}: {}", name, detail::typeToString(value)));
    if (!last) {
        out.append(", ");
    }
}

template <typename T>
    requires std::is_aggregate_v<T>
constexpr std::string toString(const T& value) {
    std::string out{"{"};
    detail::forEachFieldWithName(value, [&out](std::string_view name, const auto& field, bool last) {
        append(out, name, field, last);
    });
    out.append(" }");
    return out;
}

template <typename... Args>
constexpr std::string formatPacket(const IPacket& packet, Args&&... args) {
    return detail::formatPacketImpl(packet, std::index_sequence_for<Args...>{}, std::forward<Args>(args)...);
}

#define SCULK_FORMAT_FIELD(FIELD) std::format("{}: {}", #FIELD, formatter::detail::typeToString(FIELD))

#define SCULK_FORMAT_PACKET(...) formatter::formatPacket(*this, __VA_ARGS__);

} // namespace formatter

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE