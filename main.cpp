#include <cassert>
#include <iostream>
#include <memory>
#include <numeric>

#include <fcntl.h>

#ifdef _WIN32
#    include <io.h>
#else
#    include <unistd.h>
#endif

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
#include "mach-o_reloc.hpp"

#define BUILD_DATA_COMMAND
#define BUILD_UUID_COMMAND
#define BUILD_VERSION_COMMAND

namespace {

    template <typename To, typename From>
    std::unique_ptr<To> static_unique_pointer_cast (std::unique_ptr<From> && old) {
        return std::unique_ptr<To>{static_cast<To *> (old.release ())};
    }


    std::unique_ptr<lc_segment> build_page_zero () {
        return std::make_unique<lc_segment> (
            mach_o::seg_pagezero,
            position (0x0, std::uint64_t{1} << 32), // memory address and size of this segment
            mach_o::vm_prot_none,                   // maximum VM protection
            mach_o::vm_prot_none,                   // initial VM protection
            0x00                                    // flags
        );
    }


    std::unique_ptr<lc_segment> build_text () {

        // The 64-bit segment load command indicates that a part of this file is to be mapped into a
        // 64-bit task's address space.  If the 64-bit segment has sections then section_64
        // structures directly follow the 64-bit segment command and their size is reflected in
        // cmdsize.
        auto text_segment = std::make_unique<lc_segment> (
            mach_o::seg_text,
            position (0x0000000100000000, 0x0), // memory address and size of this segment
            mach_o::vm_prot_all,                // maximum VM protection
            mach_o::vm_prot_execute | mach_o::vm_prot_read, // initial VM protection
            0x00                                            // flags
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
                mach_o::sect_text,  // name of this section
                mach_o::seg_text,   // segment this section goes in
                0x0000000100000000, // memory address of this section
                0,                  // size in bytes of this section (patched up later)
                0,                  // file offset of this section (patched up later)
                4,                  // section alignment (power of 2)
                0,                  // file offset of relocation entries
                0,                  // number of relocation entries
                mach_o::s_attr_pure_instructions | mach_o::s_attr_some_instructions |
                    mach_o::s_regular // flags (section type and attributes)
            },
            lc_segment::contents_range (text_section_contents,
                                        text_section_contents + sizeof (text_section_contents)));
        return static_unique_pointer_cast<lc_segment> (std::move (text_segment));
    }


#ifdef BUILD_DATA_COMMAND
    std::unique_ptr<lc_segment> build_data () {
        auto data_segment = std::make_unique<lc_segment> (
            mach_o::seg_data,
            position (0x0000000200000000, 0x0),           // memory address and size of this segment
            mach_o::vm_prot_all,                          // maximum VM protection
            mach_o::vm_prot_write | mach_o::vm_prot_read, // initial VM protection
            0x00                                          // flags
        );

        static constexpr std::uint8_t data_section_contents[4096] = {
            0x0,
        };
        data_segment->add_section (
            {
                mach_o::sect_data,  // name of this section
                mach_o::seg_data,   // segment this section goes in
                0x0000000200000000, // memory address of this section
                0,                  // size in bytes of this section (patched up later)
                0,                  // file offset of this section (patched up later)
                4,                  // section alignment (power of 2)
                0,                  // file offset of relocation entries
                0,                  // number of relocation entries
                mach_o::s_regular,  // 0x80000400, // flags (section type and attributes)
            },
            lc_segment::contents_range (data_section_contents,
                                        data_section_contents + sizeof (data_section_contents)));
        return data_segment;
    }
#endif // BUILD_DATA_COMMAND

    std::unique_ptr<lc_segment> build_linkedit () {
        auto linkedit_segment = std::make_unique<lc_segment> (
            mach_o::seg_linkedit,
            position (0x0000000200001000, 0x0), // memory address and size of this segment
            mach_o::vm_prot_all,                // maximum VM protection
            mach_o::vm_prot_read,               // initial VM protection
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
        static mach_o::relocation_info ri;
        ri.r_address = 0;
        ri.r_symbolnum = 0;
        ri.r_pcrel = 0;
        ri.r_length = 0;
        ri.r_extern = 0;
        ri.r_type = 0;


        linkedit_segment->add_section (
            {
                "",                   // name of this section
                mach_o::seg_linkedit, // segment this section goes in
                0x0000000200001000,   // memory address of this section
                0,                    // size in bytes of this section (patched up later)
                0,                    // file offset of this section (patched up later)
                4,                    // section alignment (power of 2)
                0,                    // file offset of relocation entries
                0,                    // number of relocation entries
                mach_o::s_regular     // 0x80000400, // flags (section type and attributes)
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

#ifdef _WIN32
    int const fd = _open (argv[1], O_RDWR | O_CREAT | O_TRUNC);
#else
    int const fd = open (argv[1], O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
#endif
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
#ifdef BUILD_DATA_COMMAND
    commands.emplace_back (build_data ());
#endif
    commands.emplace_back (build_linkedit ()); // must be last and not writable.

    commands.emplace_back (std::make_unique<lc_dyld_info_only> ());
    commands.emplace_back (std::make_unique<lc_symtab> ());
    commands.emplace_back (std::make_unique<lc_dysymtab> ());
    commands.emplace_back (std::make_unique<lc_load_dylinker> ());
#ifdef BUILD_UUID_COMMAND
    commands.emplace_back (std::make_unique<lc_uuid> ());
#endif
#ifdef BUILD_VERSION_COMMAND
    commands.emplace_back (std::make_unique<lc_build_version> ());
#endif
    commands.emplace_back (std::make_unique<lc_main> (&text_section));
    commands.emplace_back (std::make_unique<lc_load_dylib> ("/usr/lib/libSystem.B.dylib"));
    assert (commands.size () <= reserve);

    std::size_t const total_command_size = std::accumulate (
        std::begin (commands), std::end (commands), std::size_t{0},
        [](std::size_t acc, std::unique_ptr<command> const & v) noexcept {
            return acc + v->size_bytes ();
        });
    assert (total_command_size <= type_max<std::uint32_t> ());

    mach_o::mach_header_64 header;
    header.magic = mach_o::mh_magic_64;                           // mach magic number identifier
    header.cputype = mach_o::cpu_type::x86_64;                    // cpu specifier
    header.cpusubtype = mach_o::cpu_subtype::x86_64_all;          // machine specifier
    header.filetype = mach_o::filetype_t::execute;                // type of file
    header.ncmds = narrow_cast<std::uint32_t> (commands.size ()); // number of load commands
    header.sizeofcmds =
        narrow_cast<std::uint32_t> (total_command_size); // the size of all the load commands.
    header.flags = mach_o::mh_noundefs | mach_o::mh_dyldlink | mach_o::mh_twolevel | mach_o::mh_pie;
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
