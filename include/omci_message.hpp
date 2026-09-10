#pragma once

#include <stdexcept>
#include <string>

namespace omci {

// Wraps one already-extracted OMCI message as a hex-character string (each
// character is one hex nibble; two characters = one byte in hex-text form)
// and gives named, bounds-checked access to its header fields instead of
// bare index arithmetic like v[i][8] scattered through calling code.
//
// Field layout (nibble offsets into the hex string), per the OMCI message
// header structure:
//   [0..4)   Transaction ID   (2 bytes)
//   [4..6)   Message Type     (1 byte)
//   [6..8)   Device ID        (1 byte) — not currently surfaced/used
//   [8..12)  ME Class         (2 bytes)
//   [12..16) ME Instance      (2 bytes)
//   [16..)   Content          (attribute mask + attribute values, whose
//                              exact offset depends on message type — SET
//                              and GET use one layout, SET/GET responses
//                              use another, since responses carry an
//                              extra result-code byte the requests don't)
class OmciMessage {
public:
    explicit OmciMessage(std::string hex) : hex_(std::move(hex)) {}

    const std::string& raw() const { return hex_; }
    size_t nibbleCount() const { return hex_.size(); }

    std::string transactionIdHex() const { return substr(0, 4); }
    std::string messageTypeHex() const { return substr(4, 2); }
    std::string meClassHex() const { return substr(8, 4); }
    std::string meInstanceHex() const { return substr(12, 4); }

    int meClass() const { return std::stoi(meClassHex(), nullptr, 16); }
    int meInstance() const { return std::stoi(meInstanceHex(), nullptr, 16); }

    // Bounds-checked nibble-range access for content/attribute parsing.
    // Throws std::out_of_range with a clear message on a malformed or
    // truncated line, rather than the original code's silent undefined
    // behavior from unchecked std::string::operator[].
    std::string substr(size_t nibbleOffset, size_t nibbleLength) const {
        if (nibbleOffset + nibbleLength > hex_.size()) {
            throw std::out_of_range(
                "OmciMessage: requested nibble range [" +
                std::to_string(nibbleOffset) + ", " +
                std::to_string(nibbleOffset + nibbleLength) +
                ") exceeds message length " + std::to_string(hex_.size()) +
                " — message is likely truncated or malformed");
        }
        return hex_.substr(nibbleOffset, nibbleLength);
    }

private:
    std::string hex_;
};

} // namespace omci
