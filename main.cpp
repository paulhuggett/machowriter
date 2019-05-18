#include <cassert>
#include <iostream>
#include <memory>
#include <numeric>

#include <fcntl.h>
#include <mach-o/ldsyms.h>
#include <mach-o/reloc.h>
#include <unistd.h>

#include "command.hpp"
#include "lc_build_version.hpp"
#include "lc_dyld_info_only.hpp"
#include "lc_dysymtab.hpp"
#include "lc_load_dylib.hpp"
#include "lc_load_dylinker.hpp"
#include "lc_main.hpp"
#include "lc_segment.hpp"
#include "lc_symtab.hpp"
#include "lc_uuid.hpp"

#define BUILD_DATA_COMMAND     (0)
#define BUILD_UUID_COMMAND     (0)
#define BUILD_VERSION_COMMAND  (0)

namespace {

    template <typename To, typename From>
    std::unique_ptr<To> static_unique_pointer_cast (std::unique_ptr<From> && old) {
        return std::unique_ptr<To>{static_cast<To *> (old.release ())};
    }


    std::unique_ptr<lc_segment> build_page_zero () {
        return std::make_unique<lc_segment> (
            SEG_PAGEZERO,
            position (0x0, std::uint64_t{1} << 32), // memory address and size of this segment
            vm_prot_t{0},                           // maximum VM protection
            vm_prot_t{0},                           // initial VM protection
            0x00                                    // flags
        );
    }


    std::unique_ptr<lc_segment> build_text () {

        // The 64-bit segment load command indicates that a part of this file is to be mapped into a
        // 64-bit task's address space.  If the 64-bit segment has sections then section_64
        // structures directly follow the 64-bit segment command and their size is reflected in
        // cmdsize.
        auto text_segment = std::make_unique<lc_text_segment> (
            SEG_TEXT, position (0x0000000100000000, 0x0), // memory address and size of this segment
            VM_PROT_ALL,                                  // maximum VM protection
            VM_PROT_EXECUTE | VM_PROT_READ,               // initial VM protection
            0x00                                          // flags
        );

        // 0000000000000000    pushq    %rbp
        // 0000000000000001    movq    %rsp, %rbp
        // 0000000000000004    xorl    %eax, %eax
        // 0000000000000006    popq    %rbp
        // 0000000000000007    retq
        static constexpr std::uint8_t text_section_contents[] = {
            0x55, 0x48, 0x89, 0xe5, 0x31, 0xc0, 0x5d, 0xc3,
        };
        text_segment->add_section (
            {
                SECT_TEXT,          // name of this section
                SEG_TEXT,           // segment this section goes in
                0x0000000100000000, // memory address of this section
                0,                  // size in bytes of this section (patched up later)
                0,                  // file offset of this section (patched up later)
                4,                  // section alignment (power of 2)
                0,                  // file offset of relocation entries
                0,                  // number of relocation entries
                S_ATTR_PURE_INSTRUCTIONS | S_ATTR_SOME_INSTRUCTIONS |
                    S_REGULAR, // flags (section type and attributes)
                0,             // reserved (for offset or index)
                0,             // reserved (for count or sizeof)
                0,             // reserved
            },
            lc_segment::contents_range (text_section_contents,
                                        text_section_contents + sizeof (text_section_contents)));
        return static_unique_pointer_cast<lc_segment> (std::move (text_segment));
    }

#if BUILD_DATA_COMMAND
    std::unique_ptr<lc_segment> build_data () {
        auto data_segment = std::make_unique<lc_segment> (
            SEG_DATA, position (0x0000000200000000, 0x0), // memory address and size of this segment
            VM_PROT_ALL,                                  // maximum VM protection
            VM_PROT_WRITE | VM_PROT_READ,                 // initial VM protection
            0x00                                          // flags
        );

        static constexpr std::uint8_t data_section_contents[4096] = {
            0x0,
        };
        data_segment->add_section (
            {
                SECT_DATA,          // name of this section
                SEG_DATA,           // segment this section goes in
                0x0000000200000000, // memory address of this section
                0,                  // size in bytes of this section (patched up later)
                0,                  // file offset of this section (patched up later)
                4,                  // section alignment (power of 2)
                0,                  // file offset of relocation entries
                0,                  // number of relocation entries
                S_REGULAR,          // 0x80000400, // flags (section type and attributes)
                0,                  // reserved (for offset or index)
                0,                  // reserved (for count or sizeof)
                0,                  // reserved
            },
            lc_segment::contents_range (data_section_contents,
                                        data_section_contents + sizeof (data_section_contents)));
        return data_segment;
    }
#endif // BUILD_DATA_COMMAND

    std::unique_ptr<lc_segment> build_linkedit () {
        auto linkedit_segment = std::make_unique<lc_segment> (
            SEG_LINKEDIT,
            position (0x0000000200001000, 0x0), // memory address and size of this segment
            VM_PROT_ALL,                        // maximum VM protection
            VM_PROT_READ,                       // initial VM protection
            0x00                                // flags
        );

        // struct relocation_info {
        //    int32_t r_address;         /* offset in the section to what is being
        //                      relocated */
        //    uint32_t r_symbolnum : 24, /* symbol index if r_extern == 1 or section
        //               ordinal if r_extern == 0 */
        //        r_pcrel : 1,           /* was relocated pc relative already */
        //        r_length : 2,          /* 0=byte, 1=word, 2=long, 3=quad */
        //        r_extern : 1,          /* does not include value of sym referenced */
        //        r_type : 4;            /* if not 0, machine specific relocation type */
        //};
        static relocation_info ri;
        ri.r_address = 0;
        ri.r_symbolnum = 0;
        ri.r_pcrel = 0;
        ri.r_length = 0;
        ri.r_extern = 0;
        ri.r_type = 0;


        linkedit_segment->add_section (
            {
                "",                 // name of this section
                SEG_LINKEDIT,       // segment this section goes in
                0x0000000200001000, /* memory address of this section */
                0,                  // size in bytes of this section (patched up later)
                0,                  // file offset of this section (patched up later)
                4,                  // section alignment (power of 2)
                0,                  // file offset of relocation entries
                0,                  // number of relocation entries
                S_REGULAR,          // 0x80000400, // flags (section type and attributes)
                0,                  // reserved (for offset or index)
                0,                  // reserved (for count or sizeof)
                0,                  // reserved
            },
            lc_segment::contents_range (&ri, &ri + 1));
        return linkedit_segment;
    }

} // namespace


int main (int argc, char const * argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " output-path\n";
        std::exit (EXIT_FAILURE);
    }

    int const fd = open (argv[1], O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd == -1) {
        perror ("open");
    }
    auto const scope = make_scope_guard ([fd]() { ::close (fd); });

    auto text_segment = build_text ();
    lc_segment::section_value const & text_section = (*text_segment)[0];

    constexpr auto reserve = std::size_t{12};
    std::vector<std::unique_ptr<command>> commands;
    commands.reserve (reserve);

    // segments
    commands.emplace_back (build_page_zero ());
    commands.emplace_back (std::move (text_segment));
#if BUILD_DATA_COMMAND
    commands.emplace_back (build_data ());
#endif
    commands.emplace_back (build_linkedit ()); // must be last and not writable.

    commands.emplace_back (new lc_dyld_info_only);
    commands.emplace_back (new lc_symtab);
    commands.emplace_back (new lc_dysymtab);
    commands.emplace_back (new lc_load_dylinker);
#if BUILD_UUID_COMMAND
    commands.emplace_back (new lc_uuid);
#endif
#if BUILD_VERSION_COMMAND
    commands.emplace_back (new lc_build_version);
#endif
    commands.emplace_back (new lc_main (&text_section));
    commands.emplace_back (new lc_load_dylib ("/usr/lib/libSystem.B.dylib"));
    assert (commands.size () <= reserve);

    std::size_t const total_command_size = std::accumulate (
        std::begin (commands), std::end (commands), std::size_t{0},
        [](std::size_t acc, std::unique_ptr<command> const & v) noexcept {
            return acc + v->size_bytes ();
        });
    assert (total_command_size <= type_max<std::uint32_t> ());

    mach_header_64 header;
    header.magic = MH_MAGIC_64;                                   // mach magic number identifier
    header.cputype = CPU_TYPE_X86_64;                             // cpu specifier
    header.cpusubtype = CPU_SUBTYPE_X86_64_ALL;                   // machine specifier
    header.filetype = MH_EXECUTE;                                 // type of file
    header.ncmds = narrow_cast<std::uint32_t> (commands.size ()); // number of load commands
    header.sizeofcmds =
        narrow_cast<std::uint32_t> (total_command_size); // the size of all the load commands.
    header.flags = MH_NOUNDEFS | MH_DYLDLINK | MH_TWOLEVEL | MH_PIE;
    header.reserved = 0;

    write (fd, &header, sizeof (header));

    auto const payload_start = sizeof (header) + total_command_size;
    auto payload_offset = payload_start;
    for (std::unique_ptr<command> const & v : commands) {
        assert (payload_offset % 8 == 0);
        payload_offset = v->write_command (fd, payload_offset);
    }

    assert (static_cast<std::uint64_t> (lseek (fd, 0, SEEK_CUR)) == payload_start);
    for (std::unique_ptr<command> const & v : commands) {
        v->write_payload (fd);
    }
}
