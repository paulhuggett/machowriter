#include "lc_data_in_code.hpp"

#include <cassert>
#include <cstddef>
#ifdef _WIN32
#    include <io.h>
#else
#    include <unistd.h>
#endif

#include "mach-o.hpp"

std::uint32_t lc_data_in_code::size_bytes () const noexcept {
    return sizeof (mach_o::dyld_info_command);
}

std::uint64_t lc_data_in_code::write_command (int fd, std::uint64_t offset) {
    // see <macho/loader.h> for detailed comments.
    mach_o::linkedit_data_command const cmd{
        mach_o::lc_data_in_code, sizeof (cmd),
        0, // file offset of data
        0, // file size of data in __LINKEDIT segment
    };

    assert (sizeof (cmd) % 8 == 0);
    ::write (fd, &cmd, sizeof (cmd));
    return offset;
}
