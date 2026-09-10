#pragma once

#include <string>

namespace omci {

// Converts one hex character to its 4-bit binary string representation,
// e.g. 'A' -> "1010". Returns an empty string for a non-hex character
// (the original behavior) — callers should be aware that this silently
// shortens the result rather than signaling an error; see
// hexStringToBinaryString for where this matters.
inline std::string hexCharToBinary4(char c) {
    switch (toupper(static_cast<unsigned char>(c))) {
        case '0': return "0000"; case '1': return "0001";
        case '2': return "0010"; case '3': return "0011";
        case '4': return "0100"; case '5': return "0101";
        case '6': return "0110"; case '7': return "0111";
        case '8': return "1000"; case '9': return "1001";
        case 'A': return "1010"; case 'B': return "1011";
        case 'C': return "1100"; case 'D': return "1101";
        case 'E': return "1110"; case 'F': return "1111";
        default:  return "";
    }
}

// Converts a hex string to its binary string representation, used here to
// turn a 4-hex-character (16-bit) attribute mask into 16 individual bits,
// one per possible attribute.
inline std::string hexStringToBinary(const std::string& hex) {
    std::string bin;
    bin.reserve(hex.size() * 4);
    for (char c : hex) {
        bin += hexCharToBinary4(c);
    }
    return bin;
}

} // namespace omci
