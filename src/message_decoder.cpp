#include "message_decoder.hpp"

#include <iomanip>

#include "hex_utils.hpp"
#include "omci_types.hpp"

namespace omci {

namespace {
// Nibble offsets into the message where attribute mask / content begin.
// SET and GET requests have no result-code byte, so their attribute mask
// starts 2 nibbles earlier than SET/GET *responses*, which carry one.
// These were bare magic numbers (16, 18, 20, 22) in the original code.
constexpr size_t kRequestAttrMaskOffset = 16;
constexpr size_t kRequestContentOffset = 20;
constexpr size_t kResponseAttrMaskOffset = 18;
constexpr size_t kResponseContentOffset = 22;

constexpr size_t kAttrMaskNibbleLength = 4;  // 16-bit mask, one bit per attribute
constexpr size_t kMaxAttributesPerClass = 16; // OMCI allows up to 16 attributes/ME
} // namespace

void MessageDecoder::printRawHexDump(const OmciMessage& msg, std::ostream& out) {
    // Reproduces the original's four separate loops — ranges [0,16),
    // [16,48), [48,80), [80,96) — as one generic pass. These ranges are
    // NOT uniform (8, 16, 16, 8 bytes respectively), so the line-break
    // positions below are deliberately exact, not "every 32 nibbles".
    constexpr size_t kDumpNibbleCount = 96;
    constexpr size_t kLineBreaksAt[] = {16, 48, 80, 96};
    size_t breakIdx = 0;

    for (size_t j = 0; j < kDumpNibbleCount && j + 1 < msg.nibbleCount(); j += 2) {
        if (j % 8 == 0) out << ' ';
        out << msg.substr(j, 2) << ' ';
        if (breakIdx < 4 && j + 2 == kLineBreaksAt[breakIdx]) {
            out << '\n';
            ++breakIdx;
        }
    }
}

void MessageDecoder::printAttributes(const OmciMessage& msg, std::ostream& out,
                                      size_t attrMaskNibbleOffset,
                                      size_t contentNibbleOffset,
                                      bool alsoDecodeValues) {
    std::string attrMaskHex = msg.substr(attrMaskNibbleOffset, kAttrMaskNibbleLength);
    std::string attrBits = hexStringToBinary(attrMaskHex);
    out << "attrMask <" << attrBits << ">\n";

    // A GET *request* only says which attributes are being asked for — the
    // values themselves don't exist yet, they arrive in the GET Response.
    // The mask above is still meaningful to show; there's nothing further
    // to decode until alsoDecodeValues is true (SET requests, GET Responses).
    if (!alsoDecodeValues) return;

    auto it = kClassMap.find(msg.meClass());
    if (it == kClassMap.end()) {
        out << "Class id not present in mib list\n";
        return;
    }

    out << it->second[0].name << '\n';
    size_t nibbleIndex = contentNibbleOffset;

    for (size_t j = 0; j < kMaxAttributesPerClass && j < attrBits.size(); ++j) {
        if (attrBits[j] != '1') continue;
        if (j + 1 >= it->second.size()) break; // class_map entry shorter than mask claims

        const AttributeInfo& attr = it->second[j + 1];
        size_t nibbleLen = static_cast<size_t>(attr.size) * 2;
        std::string valueHex = msg.substr(nibbleIndex, nibbleLen);
        nibbleIndex += nibbleLen;

        out << std::left << std::setw(30) << attr.name;
        if (attr.type == AttrType::Int) {
            // This branch didn't exist in the original — AttrType::Int
            // attributes (byte counters, AllocId, IntervalEndTime, etc.)
            // now render as a decimal number instead of raw hex text.
            unsigned long long value = valueHex.empty() ? 0
                : std::stoull(valueHex, nullptr, 16);
            out << value;
        } else {
            out << valueHex;
        }
        out << '\n';
    }
}

void MessageDecoder::decode(const OmciMessage& msg, std::ostream& out) {
    out << "Transaction ID <" << msg.transactionIdHex() << ">\n";

    std::string mtHex = msg.messageTypeHex();
    auto nameIt = kMessageTypeNames.find(mtHex);
    std::string mtName = (nameIt != kMessageTypeNames.end()) ? nameIt->second : "Unknown";
    out << "Message Type <" << mtHex << "> <" << mtName << ">\n";

    out << "ME Class <" << msg.meClassHex() << "><" << msg.meClass()
        << "> ME Instance <" << msg.meInstanceHex() << "><" << msg.meInstance()
        << ">\n";

    printRawHexDump(msg, out);
    out << '\n';

    if (mtHex == msgtype::kSet || mtHex == msgtype::kGet) {
        // Mask is shown for both SET and GET; values are only decoded for
        // SET, since a GET request has no values yet (they arrive in the
        // GET Response) — matches the original's exact behavior.
        printAttributes(msg, out, kRequestAttrMaskOffset, kRequestContentOffset,
                         /*alsoDecodeValues=*/mtHex == msgtype::kSet);
    } else if (mtHex == msgtype::kSetRes || mtHex == msgtype::kGetRes) {
        // Mask is shown for both responses; values are only decoded for
        // GET_RES (a Set Response carries a result code, not attribute
        // values) — matches the original's exact behavior.
        printAttributes(msg, out, kResponseAttrMaskOffset, kResponseContentOffset,
                         /*alsoDecodeValues=*/mtHex == msgtype::kGetRes);
    }

    out << "\n\n";
}

} // namespace omci
