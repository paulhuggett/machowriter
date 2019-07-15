#include "lc_load_dylib.hpp"

#include <unistd.h>

#include "mach-o.hpp"
#include "util.hpp"
#include "version.hpp"

// size_bytes
// ~~~~~~~~~~
std::uint32_t lc_load_dylib::size_bytes () const noexcept {
    std::size_t const length = name_.length ();
    return narrow_cast<std::uint32_t> (sizeof (mach_o::dylib_command) + length +
                                       calc_alignment (length, 8U));
}

// write_command
// ~~~~~~~~~~~~~
std::uint64_t lc_load_dylib::write_command (int fd, std::uint64_t offset) {
    mach_o::dylib_command cmd;
    cmd.cmd = mach_o::lc_load_dylib;   // LC_ID_DYLIB, LC_LOAD_{,WEAK_}DYLIB, LC_REEXPORT_DYLIB
    cmd.cmdsize = this->size_bytes (); // command size: includes pathname string
    cmd.dylib.name.offset = sizeof (cmd);                // library's path name
    cmd.dylib.timestamp = 2;                             // library's build time stamp
    cmd.dylib.current_version = version (1252, 200, 5);  // library's current version number
    cmd.dylib.compatibility_version = version (1, 0, 0); // library's compatibility vers number
    ::write (fd, &cmd, sizeof (cmd));

    std::size_t const length = name_.length ();
    ::write (fd, name_.c_str (), length);

    char padding[8] = {0};
    ::write (fd, padding, calc_alignment (length, 8U));
    return offset;
}
