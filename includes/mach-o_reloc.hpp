#ifndef MACH_O_RELOC_HPP
#define MACH_O_RELOC_HPP

#include <cstdint>

#ifdef __APPLE__
#    include <mach-o/reloc.h>
#    define CHECK 1
#endif

namespace mach_o {

    // Format of a relocation entry of a Mach-O file.  Modified from the 4.3BSD format.  The
    // modifications from the original format were changing the value of the r_symbolnum field for
    // "local" (r_extern == 0) relocation entries. This modification is required to support symbols
    // in an arbitrary number of sections not just the three sections (text, data and bss) in
    // a 4.3BSD file. Also the last 4 bits have had the r_type tag added to them.
    struct relocation_info {
        std::int32_t r_address; ///< offset in the section to what is being relocated
        std::uint32_t
            r_symbolnum : 24, ///< Symbol index if r_extern == 1 or section ordinal if r_extern == 0
            r_pcrel : 1,      ///< Was relocated pc relative already?
            r_length : 2,     ///< 0=byte, 1=word, 2=long, 3=quad
            r_extern : 1,     ///< Does not include value of sym referenced
            r_type : 4;       ///< If not 0, machine specific relocation type
    };

    constexpr std::uint32_t r_abs = 0; /// Absolute relocation type for Mach-O files

} // end namespace mach_o

#endif // MACH_O_RELOC_HPP
