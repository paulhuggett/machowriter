#include "lc_symtab.hpp"

#ifdef _WIN32
#    include <io.h>
#else
#    include <unistd.h>
#endif

#include "mach-o.hpp"

std::uint32_t lc_symtab::size_bytes () const noexcept {
    return sizeof (mach_o::symtab_command);
}

std::uint64_t lc_symtab::write_command (int fd, std::uint64_t offset) {
    mach_o::symtab_command const cmd{
        mach_o::lc_symtab,
        sizeof (cmd),
        0, // uint32_t symoff;  /* symbol table offset */
        0, // uint32_t nsyms;   /* number of symbol table entries */
        0, // uint32_t stroff;  /* string table offset */
        0, // uint32_t strsize; /* string table size in bytes */
    };
    ::write (fd, &cmd, sizeof (cmd));
    return offset;
}
