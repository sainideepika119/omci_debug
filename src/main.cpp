// Reads OMCI messages (as hex text already received on the host — not a
// live wire capture) for a given vendor/role profile, decodes each one,
// and writes human-readable output to a file.

#include <fstream>
#include <iostream>
#include <string>

#include "message_decoder.hpp"
#include "message_parser.hpp"

namespace {

void printUsage(const char* progName) {
    std::cerr << "Usage: " << progName
              << " <ont_realtek|ont_broadcom|olt_broadcom> [input_file] [output_file]\n"
              << "  input_file  defaults to 'omci.msg' if not given\n"
              << "  output_file defaults to 'output.txt' if not given\n";
}

} // namespace

int main(int argc, char** argv) {
    // Fix for a real bug in the original tool: argv[1] was read into a
    // std::string before checking argc, so running with no arguments at
    // all threw std::logic_error from constructing a string from a null
    // pointer and aborted (SIGABRT) instead of printing a usage message.
    if (argc < 2) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    // Fix for a second real bug: an unrecognized vendor argument used to
    // leave every vendor flag false and the program would "succeed" with
    // exit code 0, writing an output file containing nothing but a
    // timestamp — no indication anything was wrong. parseVendor() now
    // makes that an explicit, reported error instead of a silent no-op.
    auto vendor = omci::parseVendor(argv[1]);
    if (!vendor) {
        std::cerr << "Error: unrecognized vendor '" << argv[1] << "'\n\n";
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    std::string inputFile = (argc >= 3) ? argv[2] : "omci.msg";
    std::string outputFile = (argc >= 4) ? argv[3] : "output.txt";

    std::cerr << "Vendor: " << omci::vendorName(*vendor) << "\n"
              << "Input:  " << inputFile << "\n"
              << "Output: " << outputFile << "\n";

    std::vector<omci::OmciMessage> messages;
    try {
        messages = omci::MessageParser::extractMessages(inputFile, *vendor);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    if (messages.empty()) {
        std::cerr << "Warning: no messages matched the expected format for "
                   << omci::vendorName(*vendor)
                   << ". Output will be empty — check the input file and "
                      "vendor selection.\n";
    }

    std::ofstream fout(outputFile);
    if (!fout.is_open()) {
        std::cerr << "Error: could not open output file: " << outputFile << "\n";
        return EXIT_FAILURE;
    }

    time_t now = time(nullptr);
    fout << "File creation time => " << ctime(&now) << "\n\n";

    for (const auto& msg : messages) {
        try {
            omci::MessageDecoder::decode(msg, fout);
        } catch (const std::exception& e) {
            // A malformed/truncated individual message is reported and
            // skipped, rather than the original's undefined behavior from
            // unchecked indexing into a too-short string.
            std::cerr << "Warning: skipped a malformed message: " << e.what() << "\n";
        }
    }

    std::cerr << "Decoded " << messages.size() << " message(s).\n";
    return EXIT_SUCCESS;
}
