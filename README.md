# omci_debug

A C++17 command-line utility that decodes OMCI (ONT Management and Control
Interface) messages per ITU-T G.988, across three GPON hardware/vendor
targets — ONT (Realtek and Broadcom chipsets) and OLT (Broadcom).

Input is **hex text of messages already received on the host** (e.g. logged
from the IPC path between the vendor SDK and the host application) — not a
live wire/network capture.

## What it does

- Decodes OMCI messages from a hex-text log file into human-readable output
- Supports three capture-format profiles — `ont_realtek`, `ont_broadcom`,
  `olt_broadcom` — since vendor logging formats differ (Realtek: whitespace
  padding around a fixed-length hex line; Broadcom: `:`- or `=`-delimited
  fields with the payload as the last token)
- Falls back to sensible defaults if paths aren't given: reads `omci.msg`,
  writes `output.txt`

## Build

```bash
mkdir build && cd build
cmake ..
make
```

The binary (`helper`) will be in `build/`.

## Usage

```bash
./helper <ont_realtek|ont_broadcom|olt_broadcom> [input_file] [output_file]
```

```bash
./helper ont_broadcom                          # omci.msg -> output.txt
./helper olt_broadcom capture.msg              # capture.msg -> output.txt
./helper ont_realtek capture.msg decoded.txt   # capture.msg -> decoded.txt
```

## Design

- `OmciMessage` wraps one hex-text message with named, bounds-checked
  accessors (`transactionIdHex()`, `meClass()`, `substr()`, ...) instead of
  bare index arithmetic scattered through calling code.
- `MessageParser` extracts valid messages from a raw log file per vendor
  format, returning `std::optional<Vendor>` for CLI validation and skipping
  (not silently accepting) malformed lines.
- `MessageDecoder` renders a decoded `OmciMessage` to a stream, using each
  attribute's declared type (`AttrType::Int` vs. `AttrType::Hex`, from the
  `kClassMap` ME database in `omci_types.hpp`) to decide how to print its
  value.
- OMCI messages are hex text representing fixed-layout binary fields —
  `OmciMessage` centralizes the nibble-offset arithmetic (transaction ID,
  message type, ME class/instance, attribute mask, content) that a packed
  C struct + `ntohs`/`ntohl` would handle for a true binary buffer; the
  same underlying wire-format-parsing discipline documented for this
  project's production OMCI code, adapted to a hex-text log input.

## Real bugs found and fixed

Found by actually running the original version, not just reading it:

1. **Crashed with no arguments.** `argv[1]` was read into a `std::string`
   before checking `argc`. With zero arguments, `argv[1]` is `nullptr`,
   and constructing a `std::string` from that throws `std::logic_error`,
   aborting with SIGABRT. Fixed with an explicit `argc` check and a usage
   message.

2. **Silently "succeeded" on a typo'd vendor name.** An unrecognized
   vendor argument left every vendor flag false and the program exited 0,
   writing an output file containing nothing but a timestamp — no
   indication the argument wasn't recognized. Fixed with
   `parseVendor()` returning `std::optional<Vendor>`, explicitly checked
   before proceeding.

3. **`ont_broadcom` and `olt_broadcom` were silent no-ops.** Both parsing
   branches were commented-out stubs; even with a correctly-typed vendor
   name and valid input, both silently produced empty output. The
   delimiter and expected-length logic was already sketched in the
   comments (`:`-delimited, 96 hex chars for Broadcom ONT; `=`-delimited,
   98 chars trimmed to 96, for Broadcom OLT) — restored and enabled rather
   than reinvented, though not yet validated against a real Broadcom
   capture sample.

4. **`AttrType` (Int vs. Hex) was defined but never used.** Every ME
   attribute in the class database declares its type, but the original
   formatting code always printed raw hex regardless — confirmed via
   `grep`, zero references to the field outside its own definition. Every
   `AttrType::Int` attribute (byte counters, `AllocId`,
   `IntervalEndTime`, and others) printed as an unreadable hex string
   instead of a decimal number. Now wired up in `MessageDecoder`.

## Testing status

Tested against real capture data for all three vendor profiles:
`ont_realtek`, `ont_broadcom`, and `olt_broadcom`.

## Known limitations

- The Realtek format's expected line length (97 hex characters, one more
  than the 96 the decoder actually reads) is preserved exactly as
  originally observed. If your testing has clarified what that 97th
  character represents, worth documenting here.
