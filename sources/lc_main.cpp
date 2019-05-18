#include "lc_main.hpp"

#include <unistd.h>

// ctor
// ~~~~
lc_main::lc_main (not_null<lc_segment::section_value const *> main) noexcept
        : main_{main} {}

// size_bytes
// ~~~~~~~~~~
std::uint32_t lc_main::size_bytes () const noexcept {
    return sizeof (entry_point_command);
}

// write_command
// ~~~~~~~~~~~~~
std::uint64_t lc_main::write_command (int fd, std::uint64_t offset) {
    entry_point_command cmd;
    cmd.cmd = LC_MAIN; // LC_MAIN only used in MH_EXECUTE filetypes
    cmd.cmdsize = sizeof (entry_point_command);
    cmd.entryoff =
        static_cast<std::uint64_t> (main_->get_offset ()); // file (__TEXT) offset of main()
    cmd.stacksize = 0;                                     // if not zero, initial stack size
    assert (sizeof (cmd) % 8 == 0);
    ::write (fd, &cmd, sizeof (cmd));
    return offset;
}
