#include "lc_load_dylinker.hpp"

#include <mach-o/loader.h>
#include <unistd.h>

#include "util.hpp"

namespace {

    static constexpr char dylinker[] = "/usr/lib/dyld\0\0\0\0\0\0\0";
    static constexpr auto dylinker_size = array_elements (dylinker) - 1;
    static constexpr auto dylinker_cmdsize = sizeof (dylinker_command) + dylinker_size;
    static_assert (dylinker_cmdsize % 8 == 0);

} // namespace

// size_bytes
// ~~~~~~~~~~
std::uint32_t lc_load_dylinker::size_bytes () const noexcept {
    return dylinker_cmdsize;
}

// write_command
// ~~~~~~~~~~~~~
std::uint64_t lc_load_dylinker::write_command (int fd, std::uint64_t offset) {
    dylinker_command const cmd{
        LC_LOAD_DYLINKER, // LC_ID_DYLINKER, LC_LOAD_DYLINKER or LC_DYLD_ENVIRONMENT
        dylinker_cmdsize, // command size: includes pathname string
        {sizeof (cmd)}    // dynamic linker's path name
    };
    ::write (fd, &cmd, sizeof (cmd));
    ::write (fd, dylinker, dylinker_size);
    return offset;
}
