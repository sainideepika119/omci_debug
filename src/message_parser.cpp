#include "message_parser.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace omci {

std::optional<Vendor> parseVendor(const std::string& arg) {
    if (arg == "ont_realtek")  return Vendor::OntRealtek;
    if (arg == "ont_broadcom") return Vendor::OntBroadcom;
    if (arg == "olt_broadcom") return Vendor::OltBroadcom;
    return std::nullopt;
}

const char* vendorName(Vendor v) {
    switch (v) {
        case Vendor::OntRealtek:  return "ont_realtek";
        case Vendor::OntBroadcom: return "ont_broadcom";
        case Vendor::OltBroadcom: return "olt_broadcom";
    }
    return "unknown";
}

namespace {
// Realtek ONT capture: one message per line, may contain spaces/tabs as
// formatting that aren't part of the hex payload. Expected length after
// stripping whitespace is 97 hex characters — an odd count, so the final
// character is never consumed by header/content decoding (offsets used
// elsewhere only ever reach index 95). Preserved exactly as originally
// observed/working rather than "corrected" without a real capture sample
// to verify a change against.
constexpr size_t kRealtekExpectedLength = 97;

// Broadcom ONT capture: colon-delimited fields on a line, with the OMCI
// hex payload as the last field. This logic existed in the original code
// as a commented-out stub with the exact expected length already noted
// (96 hex chars = 48 bytes) — restored here rather than reinvented.
constexpr size_t kBroadcomOntExpectedLength = 96;

// Broadcom OLT capture: '='-delimited fields, hex payload as the last
// field, with 2 trailing characters (observed length 98, payload is the
// first 96) that aren't part of the decoded content — same situation as
// above, restored from the original's commented-out sketch.
constexpr size_t kBroadcomOltExpectedLength = 98;
constexpr size_t kBroadcomOltPayloadLength = 96;
} // namespace

std::optional<std::string> MessageParser::extractRealtekLine(const std::string& line) {
    std::string cleaned;
    cleaned.reserve(line.size());
    for (char ch : line) {
        if (ch != ' ' && ch != '\t') {
            cleaned += ch;
        }
    }
    if (cleaned.size() != kRealtekExpectedLength) {
        return std::nullopt;
    }
    return cleaned;
}

std::optional<std::string> MessageParser::extractBroadcomOntLine(const std::string& line) {
    // Take the last ':'-delimited field (equivalent to the original's
    // getline-in-a-loop idiom, written directly instead of relying on
    // that loop's non-obvious side effect).
    auto pos = line.rfind(':');
    std::string field = (pos == std::string::npos) ? line : line.substr(pos + 1);
    if (field.size() != kBroadcomOntExpectedLength) {
        return std::nullopt;
    }
    return field;
}

std::optional<std::string> MessageParser::extractBroadcomOltLine(const std::string& line) {
    auto pos = line.rfind('=');
    std::string field = (pos == std::string::npos) ? line : line.substr(pos + 1);
    if (field.size() != kBroadcomOltExpectedLength) {
        return std::nullopt;
    }
    return field.substr(0, kBroadcomOltPayloadLength);
}

std::vector<OmciMessage> MessageParser::extractMessages(const std::string& filepath,
                                                          Vendor vendor) {
    std::ifstream fin(filepath);
    if (!fin.is_open()) {
        throw std::runtime_error("Could not open input file: " + filepath);
    }

    std::vector<OmciMessage> messages;
    std::string line;
    while (std::getline(fin, line)) {
        std::optional<std::string> extracted;
        switch (vendor) {
            case Vendor::OntRealtek:
                extracted = extractRealtekLine(line);
                break;
            case Vendor::OntBroadcom:
                extracted = extractBroadcomOntLine(line);
                break;
            case Vendor::OltBroadcom:
                extracted = extractBroadcomOltLine(line);
                break;
        }
        if (extracted) {
            messages.emplace_back(*extracted);
        }
    }
    return messages;
}

} // namespace omci
