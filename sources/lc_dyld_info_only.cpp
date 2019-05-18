#include "lc_dyld_info_only.hpp"

#include <cassert>
#include <mach-o/loader.h>
#include <unistd.h>

std::uint32_t lc_dyld_info_only::size_bytes () const noexcept {
    return sizeof (dyld_info_command);
}

std::uint64_t lc_dyld_info_only::write_command (int fd, std::uint64_t offset) {
    // see <macho/loader.h> for detailed comments.
    dyld_info_command const cmd{
        LC_DYLD_INFO_ONLY, // LC_DYLD_INFO or LC_DYLD_INFO_ONLY
        sizeof (dyld_info_command),

        0, // file offset to rebase info
        0, // size of rebase info

        0, // file offset to binding info
        0, // size of binding info

        0, // file offset to weak binding info
        0, // size of weak binding info

        0, // file offset to lazy binding info
        0, // size of lazy binding info

        0, // file offset to lazy binding info
        0, // size of lazy binding info
    };
    assert (sizeof (cmd) % 8 == 0);
    ::write (fd, &cmd, sizeof (cmd));
    return offset;
}
