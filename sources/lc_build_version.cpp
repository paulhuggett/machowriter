#include "lc_build_version.hpp"

#include <cassert>
#include <mach-o/loader.h>
#include <unistd.h>

#include "version.hpp"

namespace {

    constexpr auto ntools = std::uint32_t{1};

    constexpr std::uint32_t command_size_bytes () noexcept {
        return sizeof (build_version_command) + ntools * sizeof (build_tool_version);
    }

} // end anonymous namespace

std::uint32_t lc_build_version::size_bytes () const noexcept {
    return command_size_bytes ();
}

std::uint64_t lc_build_version::write_command (int fd, std::uint64_t offset) {
    constexpr build_version_command cmd{
        LC_BUILD_VERSION,
        command_size_bytes (),
        PLATFORM_MACOS,      // platform
        version (10, 14, 0), // minos: X.Y.Z is encoded in nibbles xxxx.yy.zz
        version (10, 14, 0), // sdk: X.Y.Z is encoded in nibbles xxxx.yy.zz
        ntools               // number of tool entries following this
    };

    constexpr build_tool_version tools[1] = {{TOOL_LD, version (409, 12, 0)}};

    off_t prev = lseek (fd, 0, SEEK_CUR);
    ::write (fd, &cmd, sizeof (cmd));
    ::write (fd, &tools, sizeof (tools));
    assert (prev + cmd.cmdsize == lseek (fd, 0, SEEK_CUR));
    assert (cmd.cmdsize % 8 == 0);
    return offset;
}
