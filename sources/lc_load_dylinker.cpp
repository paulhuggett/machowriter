#include "lc_load_dylinker.hpp"

#ifdef _WIN32
#    include <io.h>
#else
#    include <unistd.h>
#endif

#include "mach-o.hpp"
#include "util.hpp"

namespace {

    static constexpr char dylinker[] = "/usr/lib/dyld\0\0\0\0\0\0\0";
    static constexpr auto dylinker_size = array_elements (dylinker) - 1;
    static constexpr auto dylinker_cmdsize = sizeof (mach_o::dylinker_command) + dylinker_size;
    static_assert (dylinker_cmdsize % 8 == 0, "cmdsize must be a multiple of 8");

} // namespace

// size_bytes
// ~~~~~~~~~~
std::uint32_t lc_load_dylinker::size_bytes () const noexcept {
    return dylinker_cmdsize;
}

// write_command
// ~~~~~~~~~~~~~
std::uint64_t lc_load_dylinker::write_command (int fd, std::uint64_t offset) {
    mach_o::dylinker_command const cmd{
        mach_o::lc_load_dylinker, // LC_ID_DYLINKER, LC_LOAD_DYLINKER or LC_DYLD_ENVIRONMENT
        dylinker_cmdsize,         // command size: includes pathname string
        {sizeof (cmd)}            // dynamic linker's path name
    };
    ::write (fd, &cmd, sizeof (cmd));
    ::write (fd, dylinker, dylinker_size);
    return offset;
}
