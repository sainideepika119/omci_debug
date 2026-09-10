#pragma once

#include <optional>
#include <string>
#include <vector>

#include "omci_message.hpp"

namespace omci {

enum class Vendor { OntRealtek, OntBroadcom, OltBroadcom };

// Parses a CLI vendor argument into a Vendor, or nullopt if unrecognized.
// This is the fix for a real bug in the original tool: an unrecognized
// argument (a typo, say) left every vendor flag false and the program
// "succeeded" with exit code 0, writing an output file containing nothing
// but a timestamp — no indication anything was wrong. Callers must now
// handle nullopt explicitly instead of silently proceeding.
std::optional<Vendor> parseVendor(const std::string& arg);

const char* vendorName(Vendor v);

class MessageParser {
public:
    // Reads filepath line by line and extracts every valid OMCI message
    // for the given vendor's capture format. Malformed/short lines are
    // skipped (not silently accepted as garbage input). Throws
    // std::runtime_error if the file can't be opened — the original code
    // checked this for the output file but not the input file.
    static std::vector<OmciMessage> extractMessages(const std::string& filepath,
                                                      Vendor vendor);

private:
    // Each of these returns the cleaned hex-string message from one raw
    // line, or nullopt if the line doesn't match this vendor's expected
    // capture format (wrong length, wrong delimiter, blank line, etc).
    static std::optional<std::string> extractRealtekLine(const std::string& line);
    static std::optional<std::string> extractBroadcomOntLine(const std::string& line);
    static std::optional<std::string> extractBroadcomOltLine(const std::string& line);
};

} // namespace omci
