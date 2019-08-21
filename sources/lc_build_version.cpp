#include "lc_build_version.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#ifdef _WIN32
#    include <io.h>
#else
#    include <unistd.h>
#endif

#include "mach-o.hpp"
#include "version.hpp"

namespace {

    constexpr auto ntools = std::uint32_t{1};

    constexpr std::uint32_t command_size_bytes () noexcept {
        return sizeof (mach_o::build_version_command) +
               ntools * sizeof (mach_o::build_tool_version);
    }

} // end anonymous namespace

std::uint32_t lc_build_version::size_bytes () const noexcept {
    return command_size_bytes ();
}


std::uint64_t lc_build_version::write_command (int fd, std::uint64_t offset) {
    constexpr mach_o::build_version_command cmd{
        mach_o::lc_build_version,
        command_size_bytes (),
        mach_o::platform_macos, // platform
        version (10, 14, 0),    // minos: X.Y.Z is encoded in nibbles xxxx.yy.zz
        version (10, 14, 0),    // sdk: X.Y.Z is encoded in nibbles xxxx.yy.zz
        ntools                  // number of tool entries following this
    };

    constexpr mach_o::build_tool_version tools[1] = {{mach_o::tool_ld, version (409, 12, 0)}};

    off_t prev = lseek (fd, 0, SEEK_CUR);
    ::write (fd, &cmd, sizeof (cmd));
    ::write (fd, &tools, sizeof (tools));
    assert (prev + cmd.cmdsize == lseek (fd, 0, SEEK_CUR));
    assert (cmd.cmdsize % 8 == 0);
    return offset;
}
