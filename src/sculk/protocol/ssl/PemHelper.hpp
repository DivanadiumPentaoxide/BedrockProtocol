#pragma once
#include <string>
#include <string_view>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE::pem_helper {

[[nodiscard]] inline std::string_view trimPemContent(std::string_view pem) {
    constexpr std::string_view whitespace = " \t\r\n";
    auto                       begin      = pem.find_first_not_of(whitespace);
    if (begin == std::string_view::npos) {
        return {};
    }
    auto end = pem.find_last_not_of(whitespace);
    return pem.substr(begin, end - begin + 1);
}

[[nodiscard]] inline std::string_view stripPemMarkers(std::string_view pem) {
    auto content = trimPemContent(pem);
    if (content.empty()) {
        return {};
    }

    if (!content.starts_with("-----BEGIN ")) {
        return content;
    }

    auto beginLineEnd = content.find('\n');
    if (beginLineEnd == std::string_view::npos) {
        return content;
    }

    auto lastLineStart = content.rfind('\n');
    if (lastLineStart == std::string_view::npos || lastLineStart <= beginLineEnd) {
        return content;
    }

    auto endLine = trimPemContent(content.substr(lastLineStart + 1));
    if (!endLine.starts_with("-----END ")) {
        return content;
    }

    return trimPemContent(content.substr(beginLineEnd + 1, lastLineStart - beginLineEnd - 1));
}

[[nodiscard]] inline std::string stripPemMarkersAndCompact(std::string_view pem) {
    auto body = stripPemMarkers(pem);
    if (body.empty()) {
        return {};
    }

    constexpr std::string_view whitespace = " \t\r\n";
    if (body.find_first_of(whitespace) == std::string_view::npos) {
        return std::string(body);
    }

    std::string compacted;
    compacted.reserve(body.size());
    for (char ch : body) {
        if (whitespace.find(ch) == std::string_view::npos) {
            compacted.push_back(ch);
        }
    }
    return compacted;
}

[[nodiscard]] inline std::string_view normalizePemForRead(std::string_view pem, bool isPrivate, std::string& ownedPem) {
    auto trimmedPem = trimPemContent(pem);
    if (trimmedPem.empty()) {
        return {};
    }

    if (trimmedPem.find("-----BEGIN ") != std::string_view::npos) {
        return trimmedPem;
    }

    ownedPem.clear();
    if (isPrivate) {
        ownedPem.reserve(trimmedPem.size() + 54);
        ownedPem.append("-----BEGIN PRIVATE KEY-----\n");
        ownedPem.append(trimmedPem);
        ownedPem.append("\n-----END PRIVATE KEY-----\n");
    } else {
        ownedPem.reserve(trimmedPem.size() + 52);
        ownedPem.append("-----BEGIN PUBLIC KEY-----\n");
        ownedPem.append(trimmedPem);
        ownedPem.append("\n-----END PUBLIC KEY-----\n");
    }

    return ownedPem;
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE::pem_helper