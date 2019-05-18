#include "lc_data_in_code.hpp"

#include <cassert>
#include <mach-o/loader.h>
#include <unistd.h>

std::uint32_t lc_data_in_code::size_bytes () const noexcept {
    return sizeof (dyld_info_command);
}

std::uint64_t lc_data_in_code::write_command (int fd, std::uint64_t offset) {
    // see <macho/loader.h> for detailed comments.
    linkedit_data_command const cmd{
        LC_DATA_IN_CODE, sizeof (cmd),
        0, // file offset of data
        0, // file size of data in __LINKEDIT segment
    };

    assert (sizeof (cmd) % 8 == 0);
    ::write (fd, &cmd, sizeof (cmd));
    return offset;
}
