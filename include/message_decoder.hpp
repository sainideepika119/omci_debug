#pragma once

#include <ostream>

#include "omci_message.hpp"

namespace omci {

class MessageDecoder {
public:
    // Decodes one message and writes a human-readable representation to
    // the given stream: transaction ID, message type, ME class/instance,
    // a raw hex dump, the attribute mask, and decoded attribute values.
    //
    // Unlike the original tool, this actually uses each attribute's
    // AttrType (Int vs. Hex) to decide how to render its value — the
    // original defined this field in every class_map entry but never
    // read it, so every attribute printed as raw hex regardless of type.
    static void decode(const OmciMessage& msg, std::ostream& out);

private:
    static void printRawHexDump(const OmciMessage& msg, std::ostream& out);
    static void printAttributes(const OmciMessage& msg, std::ostream& out,
                                 size_t attrMaskNibbleOffset,
                                 size_t contentNibbleOffset,
                                 bool alsoDecodeValues);
};

} // namespace omci
