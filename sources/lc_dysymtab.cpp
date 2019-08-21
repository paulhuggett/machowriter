#include "lc_dysymtab.hpp"

#ifdef _WIN32
#    include <io.h>
#else
#    include <unistd.h>
#endif

#include "mach-o.hpp"

std::uint32_t lc_dysymtab::size_bytes () const noexcept {
    return sizeof (mach_o::dysymtab_command);
}

std::uint64_t lc_dysymtab::write_command (int fd, std::uint64_t offset) {
    mach_o::dysymtab_command const cmd{
        mach_o::lc_dysymtab,
        sizeof (cmd),

        0, // uint32_t ilocalsym;    /* index to local symbols */
        0, // uint32_t nlocalsym;    /* number of local symbols */

        0, // uint32_t iextdefsym;/* index to externally defined symbols */
        0, // uint32_t nextdefsym;/* number of externally defined symbols */

        0, // uint32_t iundefsym;    /* index to undefined symbols */
        0, // uint32_t nundefsym;    /* number of undefined symbols */

        0, // uint32_t tocoff;    /* file offset to table of contents */
        0, // uint32_t ntoc;    /* number of entries in table of contents */

        0, // uint32_t modtaboff;    /* file offset to module table */
        0, // uint32_t nmodtab;    /* number of module table entries */

        0, // uint32_t extrefsymoff;    /* offset to referenced symbol table */
        0, // uint32_t nextrefsyms;    /* number of referenced symbol table entries */

        0, // uint32_t indirectsymoff; /* file offset to the indirect symbol table */
        0, // uint32_t nindirectsyms;  /* number of indirect symbol table entries */

        0, // uint32_t extreloff;    /* offset to external relocation entries */
        0, // uint32_t nextrel;    /* number of external relocation entries */

        0, // uint32_t locreloff;    /* offset to local relocation entries */
        0, // uint32_t nlocrel;    /* number of local relocation entries */
    };
    ::write (fd, &cmd, sizeof (cmd));
    return offset;
}
