#include "lc_uuid.hpp"

#include <algorithm>
#include <random>
#ifdef _WIN32
#    include <io.h>
#else
#    include <unistd.h>
#endif

#include "mach-o.hpp"
#include "util.hpp"

// size_bytes
// ~~~~~~~~~~
std::uint32_t lc_uuid::size_bytes () const noexcept {
    return sizeof (mach_o::uuid_command);
}

// write_command
// ~~~~~~~~~~~~~
std::uint64_t lc_uuid::write_command (int fd, std::uint64_t offset) {
    enum {
        version_octet = 6,
        variant_octet = 8,
    };

    mach_o::uuid_command cmd;
    cmd.cmd = mach_o::lc_uuid;
    cmd.cmdsize = sizeof (cmd);
    std::generate (cmd.uuid, cmd.uuid + array_elements (cmd.uuid), [] () {
        static std::random_device device;
        static std::mt19937_64 generator (device ());
        static std::uniform_int_distribution<std::uint8_t> distribution;
        return distribution (generator);
    });
    // Set variant: must be 0b10xxxxxx
    cmd.uuid[variant_octet] &= 0xBF; // 0b10111111;
    cmd.uuid[variant_octet] |= 0x80; // 0b10000000;

    // Set version: must be 0b0100xxxx
    cmd.uuid[version_octet] &= 0x4F;               // 0b01001111;
    cmd.uuid[version_octet] |= std::uint8_t{0x40}; // a random number based UUID.

    ::write (fd, &cmd, sizeof (cmd));
    return offset;
}
