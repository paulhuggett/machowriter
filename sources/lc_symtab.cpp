#include "lc_symtab.hpp"

#include <mach-o/loader.h>
#include <unistd.h>

std::uint32_t lc_symtab::size_bytes () const noexcept {
    return sizeof (symtab_command);
}

std::uint64_t lc_symtab::write_command (int fd, std::uint64_t offset) {
    symtab_command const cmd{
        LC_SYMTAB, sizeof (cmd),
        0, // uint32_t symoff;  /* symbol table offset */
        0, // uint32_t nsyms;   /* number of symbol table entries */
        0, // uint32_t stroff;  /* string table offset */
        0, // uint32_t strsize; /* string table size in bytes */
    };
    ::write (fd, &cmd, sizeof (cmd));
    return offset;
}
