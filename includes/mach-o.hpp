#ifndef MACH_O_HPP
#define MACH_O_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>

#if __APPLE__
#    include <mach-o/loader.h>
#    define CHECK 1
#endif

#define STATIC_ASSERT(x) static_assert (x, #x)

namespace mach_o {

    using cpu_threadtype_t = std::uint32_t;

    /*
     * Capability bits used in the definition of cpu_type.
     */
    constexpr std::uint32_t cpu_arch_mask = 0xff000000;  // mask for architecture bits
    constexpr std::uint32_t cpu_arch_abi64 = 0x01000000; // 64 bit ABI
    constexpr std::uint32_t cpu_arch_abi64_32 =
        0x02000000; // ABI for 64-bit hardware with 32-bit types; LP32


    enum class cpu_type : std::uint32_t {
        any = static_cast<std::uint32_t> (-1),

        vax = 1,
        // skip                ((t) 2)
        // skip                ((t) 3)
        // skip                ((t) 4)
        // skip                ((t) 5)
        mc680x0 = 6,
        x86 = 7,
        i386 = x86, // compatibility
        x86_64 = x86 | cpu_arch_abi64,

        // skip MIPS        ((t) 8)
        // skip                         ((t) 9)
        mc98000 = 10,
        hppa = 11,
        arm = 12,
        arm64 = arm | cpu_arch_abi64,
        arm64_32 = arm | cpu_arch_abi64_32,
        mc88000 = 13,
        sparc = 14,
        i860 = 15,
        // skip    ALPHA        ((t) 16)
        // skip                ((t) 17)
        powerpc = 18,
        powerpc64 = powerpc | cpu_arch_abi64,
    };

#ifdef CHECK
    STATIC_ASSERT (static_cast<std::uint32_t> (cpu_type::any) == CPU_TYPE_ANY);
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_type::vax) == CPU_TYPE_VAX);
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_type::mc680x0) == CPU_TYPE_MC680x0);
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_type::x86) == CPU_TYPE_X86);
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_type::i386) == CPU_TYPE_I386);
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_type::x86_64) == CPU_TYPE_X86_64);
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_type::mc98000) == CPU_TYPE_MC98000);
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_type::hppa) == CPU_TYPE_HPPA);
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_type::arm) == CPU_TYPE_ARM);
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_type::arm64) == CPU_TYPE_ARM64);
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_type::arm64_32) == CPU_TYPE_ARM64_32);
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_type::mc88000) == CPU_TYPE_MC88000);
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_type::sparc) == CPU_TYPE_SPARC);
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_type::i860) == CPU_TYPE_I860);
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_type::powerpc) == CPU_TYPE_POWERPC);
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_type::powerpc64) == CPU_TYPE_POWERPC64);
#endif // CHECK

    enum class cpu_subtype : std::uint32_t {
        // X86 subtypes.
        x86_all = 3,
        x86_64_all = 3,
        x86_arch1 = 4,
        x86_64_h = 8, // Haswell feature subset
    };

#ifdef CHECK
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_subtype::x86_all) == CPU_SUBTYPE_X86_ALL);
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_subtype::x86_64_all) == CPU_SUBTYPE_X86_64_ALL);
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_subtype::x86_arch1) == CPU_SUBTYPE_X86_ARCH1);
     STATIC_ASSERT (static_cast<std::uint32_t> (cpu_subtype::x86_64_h) == CPU_SUBTYPE_X86_64_H);
#endif // CHECK
    /*
     * The layout of the file depends on the filetype.  For all but the MH_OBJECT
     * file type the segments are padded out and aligned on a segment alignment
     * boundary for efficient demand pageing.  The MH_EXECUTE, MH_FVMLIB, MH_DYLIB,
     * MH_DYLINKER and MH_BUNDLE file types also have the headers included as part
     * of their first segment.
     *
     * The file type MH_OBJECT is a compact format intended as output of the
     * assembler and input (and possibly output) of the link editor (the .o
     * format).  All sections are in one unnamed segment with no segment padding.
     * This format is used as an executable format when the file is so small the
     * segment padding greatly increases its size.
     *
     * The file type MH_PRELOAD is an executable format intended for things that
     * are not executed under the kernel (proms, stand alones, kernels, etc).  The
     * format can be executed under the kernel but may demand paged it and not
     * preload it before execution.
     *
     * A core file is in MH_CORE format and can be any in an arbritray legal
     * Mach-O file.
     */
    enum class filetype_t : std::uint32_t {
        object = 0x1,      // relocatable object file
        execute = 0x2,     // demand paged executable file
        fvmlib = 0x3,      // fixed VM shared library file
        core = 0x4,        // core file
        preload = 0x5,     // preloaded executable file
        dylib = 0x6,       // dynamically bound shared library
        dylinker = 0x7,    // dynamic link editor
        bundle = 0x8,      // dynamically bound bundle file
        dylib_stub = 0x9,  // shared library stub for static linking only, no section contents
        dsym = 0xa,        // companion file with only debug sections
        kext_bundle = 0xb, // x86_64 kexts
    };

#ifdef CHECK
     STATIC_ASSERT (static_cast<std::uint32_t> (filetype_t::object) == MH_OBJECT);
     STATIC_ASSERT (static_cast<std::uint32_t> (filetype_t::execute) == MH_EXECUTE);
     STATIC_ASSERT (static_cast<std::uint32_t> (filetype_t::fvmlib) == MH_FVMLIB);
     STATIC_ASSERT (static_cast<std::uint32_t> (filetype_t::core) == MH_CORE);
     STATIC_ASSERT (static_cast<std::uint32_t> (filetype_t::preload) == MH_PRELOAD);
     STATIC_ASSERT (static_cast<std::uint32_t> (filetype_t::dylib) == MH_DYLIB);
     STATIC_ASSERT (static_cast<std::uint32_t> (filetype_t::dylinker) == MH_DYLINKER);
     STATIC_ASSERT (static_cast<std::uint32_t> (filetype_t::bundle) == MH_BUNDLE);
     STATIC_ASSERT (static_cast<std::uint32_t> (filetype_t::dylib_stub) == MH_DYLIB_STUB);
     STATIC_ASSERT (static_cast<std::uint32_t> (filetype_t::dsym) == MH_DSYM);
     STATIC_ASSERT (static_cast<std::uint32_t> (filetype_t::kext_bundle) == MH_KEXT_BUNDLE);
#endif // CHECK

    // Constants for the flags field of the mach_header
    enum : std::uint32_t {
        // the object file has no undefined references
        mh_noundefs = 0x1,
        // the object file is the output of an incremental link against a base file and can't be
        // link edited again
        mh_incrlink = 0x2,
        // the object file is input for the dynamic linker and can't be staticly link edited again
        mh_dyldlink = 0x4,
        // the object file's undefined references are bound by the dynamic linker when loaded.
        mh_bindatload = 0x8,
        // the file has its dynamic undefined references prebound.
        mh_prebound = 0x10,
        // the file has its read-only and read-write segments split
        mh_split_segs = 0x20,
        // the shared library init routine is to be run lazily via catching memory faults to its
        // writeable segments (obsolete)
        mh_lazy_init = 0x40,
        // the image is using two-level name space bindings
        mh_twolevel = 0x80,
        // the executable is forcing all images to use flat name space bindings
        mh_force_flat = 0x100,
        // this umbrella guarantees no multiple defintions of symbols in its sub-images so the
        // two-level namespace hints can always be used.
        mh_nomultidefs = 0x200,
        // do not have dyld notify the prebinding agent about this executable
        mh_nofixprebinding = 0x400,
        // the binary is not prebound but can have its prebinding redone. only used when MH_PREBOUND
        // is not set.
        mh_prebindable = 0x800,
        // indicates that this binary binds to all two-level namespace modules of its dependent
        // libraries. only used when MH_PREBINDABLE and MH_TWOLEVEL are both set.
        mh_allmodsbound = 0x1000,
        // safe to divide up the sections into sub-sections via symbols for dead code stripping
        mh_subsections_via_symbols = 0x2000,
        // the binary has been canonicalized via the unprebind operation
        mh_canonical = 0x4000,
        // the final linked image contains external weak symbols
        mh_weak_defines = 0x8000,
        // the final linked image uses weak symbols
        mh_binds_to_weak = 0x10000,
        // When this bit is set, all stacks in the task will be given stack execution privilege.
        // Only used in MH_EXECUTE filetypes.
        mh_allow_stack_execution = 0x20000,
        // When this bit is set, the binary declares it is safe for use in processes with uid zero
        mh_root_safe = 0x40000,
        // When this bit is set, the binary declares it is safe for use in processes when
        // issetugid() is true
        mh_setuid_safe = 0x80000,
        // When this bit is set on a dylib, the static linker does not need to examine dependent
        // dylibs to see if any are re-exported
        mh_no_reexported_dylibs = 0x100000,
        // When this bit is set, the OS will load the main executable at a random address.  Only
        // used in MH_EXECUTE filetypes.
        mh_pie = 0x200000,
        // Only for use on dylibs.  When linking against a dylib that has this bit set, the static
        // linker will automatically not create a LC_LOAD_DYLIB load command to the dylib if no
        // symbols are being referenced from the dylib.
        mh_dead_strippable_dylib = 0x400000,
        // Contains a section of type S_THREAD_LOCAL_VARIABLES
        mh_has_tlv_descriptors = 0x800000,
        // When this bit is set, the OS will run the main executable with a non-executable heap even
        // on platforms (e.g. i386) that don't require it. Only used in MH_EXECUTE filetypes.
        mh_no_heap_execution = 0x1000000,
        // The code was linked for use in an application extension.
        mh_app_extension_safe = 0x02000000,
        // The external symbols listed in the nlist symbol table do not include all the symbols
        // listed in the dyld info.
        mh_nlist_outofsync_with_dyldinfo = 0x04000000,
        mh_sim_support = 0x08000000,
    };

#ifdef CHECK
     STATIC_ASSERT (mh_noundefs == MH_NOUNDEFS);
     STATIC_ASSERT (mh_incrlink == MH_INCRLINK);
     STATIC_ASSERT (mh_dyldlink == MH_DYLDLINK);
     STATIC_ASSERT (mh_bindatload == MH_BINDATLOAD);
     STATIC_ASSERT (mh_prebound == MH_PREBOUND);
     STATIC_ASSERT (mh_split_segs == MH_SPLIT_SEGS);
     STATIC_ASSERT (mh_lazy_init == MH_LAZY_INIT);
     STATIC_ASSERT (mh_twolevel == MH_TWOLEVEL);
     STATIC_ASSERT (mh_force_flat == MH_FORCE_FLAT);
     STATIC_ASSERT (mh_nomultidefs == MH_NOMULTIDEFS);
     STATIC_ASSERT (mh_nofixprebinding == MH_NOFIXPREBINDING);
     STATIC_ASSERT (mh_prebindable == MH_PREBINDABLE);
     STATIC_ASSERT (mh_allmodsbound == MH_ALLMODSBOUND);
     STATIC_ASSERT (mh_subsections_via_symbols == MH_SUBSECTIONS_VIA_SYMBOLS);
     STATIC_ASSERT (mh_canonical == MH_CANONICAL);
     STATIC_ASSERT (mh_weak_defines == MH_WEAK_DEFINES);
     STATIC_ASSERT (mh_binds_to_weak == MH_BINDS_TO_WEAK);
     STATIC_ASSERT (mh_allow_stack_execution == MH_ALLOW_STACK_EXECUTION);
     STATIC_ASSERT (mh_root_safe == MH_ROOT_SAFE);
     STATIC_ASSERT (mh_setuid_safe == MH_SETUID_SAFE);
     STATIC_ASSERT (mh_no_reexported_dylibs == MH_NO_REEXPORTED_DYLIBS);
     STATIC_ASSERT (mh_pie == MH_PIE);
     STATIC_ASSERT (mh_dead_strippable_dylib == MH_DEAD_STRIPPABLE_DYLIB);
     STATIC_ASSERT (mh_has_tlv_descriptors == MH_HAS_TLV_DESCRIPTORS);
     STATIC_ASSERT (mh_no_heap_execution == MH_NO_HEAP_EXECUTION);
     STATIC_ASSERT (mh_app_extension_safe == MH_APP_EXTENSION_SAFE);
     STATIC_ASSERT (mh_nlist_outofsync_with_dyldinfo == MH_NLIST_OUTOFSYNC_WITH_DYLDINFO);
     STATIC_ASSERT (mh_sim_support == MH_SIM_SUPPORT);
#endif // CHECK

    // The 64-bit mach header appears at the very beginning of object files for
    // 64-bit architectures.
    struct mach_header_64 {
        std::uint32_t magic;      // mach magic number identifier
        cpu_type cputype;         // cpu specifier
        cpu_subtype cpusubtype;   // machine specifier
        filetype_t filetype;      // type of file
        std::uint32_t ncmds;      // number of load commands
        std::uint32_t sizeofcmds; // the size of all the load commands
        std::uint32_t flags;      // flags
        std::uint32_t reserved;   // reserved
    };

#ifdef CHECK
     STATIC_ASSERT (sizeof (mach_header_64) == sizeof (::mach_header_64));
     STATIC_ASSERT (offsetof (mach_header_64, magic) == offsetof (::mach_header_64, magic));
     STATIC_ASSERT (offsetof (mach_header_64, cputype) == offsetof (::mach_header_64, cputype));
     STATIC_ASSERT (offsetof (mach_header_64, cpusubtype) ==
                   offsetof (::mach_header_64, cpusubtype));
     STATIC_ASSERT (offsetof (mach_header_64, filetype) == offsetof (::mach_header_64, filetype));
     STATIC_ASSERT (offsetof (mach_header_64, ncmds) == offsetof (::mach_header_64, ncmds));
     STATIC_ASSERT (offsetof (mach_header_64, sizeofcmds) ==
                   offsetof (::mach_header_64, sizeofcmds));
     STATIC_ASSERT (offsetof (mach_header_64, flags) == offsetof (::mach_header_64, flags));
     STATIC_ASSERT (offsetof (mach_header_64, reserved) == offsetof (::mach_header_64, reserved));
#else
     STATIC_ASSERT (sizeof (mach_header_64) == 32);
     STATIC_ASSERT (offsetof (mach_header_64, magic) == 0);
     STATIC_ASSERT (offsetof (mach_header_64, cputype) == 4);
     STATIC_ASSERT (offsetof (mach_header_64, cpusubtype) == 8);
     STATIC_ASSERT (offsetof (mach_header_64, filetype) == 12);
     STATIC_ASSERT (offsetof (mach_header_64, ncmds) == 16);
     STATIC_ASSERT (offsetof (mach_header_64, sizeofcmds) == 20);
     STATIC_ASSERT (offsetof (mach_header_64, flags) == 24);
     STATIC_ASSERT (offsetof (mach_header_64, reserved) == 28);
#endif // CHECK

    // Constant for the magic field of the mach_header_64 (64-bit architectures)
    constexpr std::uint32_t mh_magic_64 = 0xfeedfacf; // the 64-bit mach magic number
    constexpr std::uint32_t mh_cigam_64 = 0xcffaedfe; // NXSwapInt(MH_MAGIC_64)

#ifdef CHECK
     STATIC_ASSERT (mh_magic_64 == MH_MAGIC_64);
     STATIC_ASSERT (mh_cigam_64 == MH_CIGAM_64);
#endif


    // A variable length string in a load command is represented by an lc_str union. The strings
    // are stored just after the load command structure and the offset is from the start of the load
    // command structure. The size of the string is reflected in the cmdsize field of the load
    // command. Once again any padded bytes to bring the cmdsize field to a multiple of 4 bytes must
    // be zero.
    union lc_str {
        std::uint32_t offset; // offset to the string
    };



    // The dyld_info_command contains the file offsets and sizes of the new compressed form of the
    // information dyld needs to load the image.  This information is used by dyld on Mac OS X 10.6
    // and later.  All information pointed to by this command is encoded using byte streams, so no
    // endian swapping is needed to interpret it.
    struct dyld_info_command {
        std::uint32_t cmd;     // LC_DYLD_INFO or LC_DYLD_INFO_ONLY
        std::uint32_t cmdsize; // sizeof(struct dyld_info_command)

        /*
         * Dyld rebases an image whenever dyld loads it at an address different
         * from its preferred address.  The rebase information is a stream
         * of byte sized opcodes whose symbolic names start with REBASE_OPCODE_.
         * Conceptually the rebase information is a table of tuples:
         *    <seg-index, seg-offset, type>
         * The opcodes are a compressed way to encode the table by only
         * encoding when a column changes.  In addition simple patterns
         * like "every n'th offset for m times" can be encoded in a few
         * bytes.
         */
        std::uint32_t rebase_off;  // file offset to rebase info
        std::uint32_t rebase_size; // size of rebase info

        /*
         * Dyld binds an image during the loading process, if the image
         * requires any pointers to be initialized to symbols in other images.
         * The bind information is a stream of byte sized
         * opcodes whose symbolic names start with BIND_OPCODE_.
         * Conceptually the bind information is a table of tuples:
         *    <seg-index, seg-offset, type, symbol-library-ordinal, symbol-name, addend>
         * The opcodes are a compressed way to encode the table by only
         * encoding when a column changes.  In addition simple patterns
         * like for runs of pointers initialzed to the same value can be
         * encoded in a few bytes.
         */
        std::uint32_t bind_off;  // file offset to binding info
        std::uint32_t bind_size; // size of binding info

        /*
         * Some C++ programs require dyld to unique symbols so that all
         * images in the process use the same copy of some code/data.
         * This step is done after binding. The content of the weak_bind
         * info is an opcode stream like the bind_info.  But it is sorted
         * alphabetically by symbol name.  This enable dyld to walk
         * all images with weak binding information in order and look
         * for collisions.  If there are no collisions, dyld does
         * no updating.  That means that some fixups are also encoded
         * in the bind_info.  For instance, all calls to "operator new"
         * are first bound to libstdc++.dylib using the information
         * in bind_info.  Then if some image overrides operator new
         * that is detected when the weak_bind information is processed
         * and the call to operator new is then rebound.
         */
        std::uint32_t weak_bind_off;  // file offset to weak binding info
        std::uint32_t weak_bind_size; // size of weak binding info

        /*
         * Some uses of external symbols do not need to be bound immediately.
         * Instead they can be lazily bound on first use.  The lazy_bind
         * are contains a stream of BIND opcodes to bind all lazy symbols.
         * Normal use is that dyld ignores the lazy_bind section when
         * loading an image.  Instead the static linker arranged for the
         * lazy pointer to initially point to a helper function which
         * pushes the offset into the lazy_bind area for the symbol
         * needing to be bound, then jumps to dyld which simply adds
         * the offset to lazy_bind_off to get the information on what
         * to bind.
         */
        std::uint32_t lazy_bind_off;  // file offset to lazy binding info
        std::uint32_t lazy_bind_size; // size of lazy binding infs

        /*
         * The symbols exported by a dylib are encoded in a trie.  This
         * is a compact representation that factors out common prefixes.
         * It also reduces LINKEDIT pages in RAM because it encodes all
         * information (name, address, flags) in one small, contiguous range.
         * The export area is a stream of nodes.  The first node sequentially
         * is the start node for the trie.
         *
         * Nodes for a symbol start with a uleb128 that is the length of
         * the exported symbol information for the string so far.
         * If there is no exported symbol, the node starts with a zero byte.
         * If there is exported info, it follows the length.
         *
         * First is a uleb128 containing flags. Normally, it is followed by
         * a uleb128 encoded offset which is location of the content named
         * by the symbol from the mach_header for the image.  If the flags
         * is EXPORT_SYMBOL_FLAGS_REEXPORT, then following the flags is
         * a uleb128 encoded library ordinal, then a zero terminated
         * UTF8 string.  If the string is zero length, then the symbol
         * is re-export from the specified dylib with the same name.
         * If the flags is EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER, then following
         * the flags is two uleb128s: the stub offset and the resolver offset.
         * The stub is used by non-lazy pointers.  The resolver is used
         * by lazy pointers and must be called to get the actual address to use.
         *
         * After the optional exported symbol information is a byte of
         * how many edges (0-255) that this node has leaving it,
         * followed by each edge.
         * Each edge is a zero terminated UTF8 of the addition chars
         * in the symbol, followed by a uleb128 offset for the node that
         * edge points to.
         */
        std::uint32_t export_off;  // file offset to lazy binding info
        std::uint32_t export_size; // size of lazy binding infs
    };

#ifdef CHECK
     STATIC_ASSERT (sizeof (dyld_info_command) == sizeof (::dyld_info_command));
     STATIC_ASSERT (offsetof (dyld_info_command, cmd) == offsetof (::dyld_info_command, cmd));
     STATIC_ASSERT (offsetof (dyld_info_command, cmdsize) ==
                   offsetof (::dyld_info_command, cmdsize));
     STATIC_ASSERT (offsetof (dyld_info_command, rebase_off) ==
                   offsetof (::dyld_info_command, rebase_off));
     STATIC_ASSERT (offsetof (dyld_info_command, rebase_size) ==
                   offsetof (::dyld_info_command, rebase_size));
     STATIC_ASSERT (offsetof (dyld_info_command, bind_off) ==
                   offsetof (::dyld_info_command, bind_off));
     STATIC_ASSERT (offsetof (dyld_info_command, bind_size) ==
                   offsetof (::dyld_info_command, bind_size));
     STATIC_ASSERT (offsetof (dyld_info_command, weak_bind_off) ==
                   offsetof (::dyld_info_command, weak_bind_off));
     STATIC_ASSERT (offsetof (dyld_info_command, weak_bind_size) ==
                   offsetof (::dyld_info_command, weak_bind_size));
     STATIC_ASSERT (offsetof (dyld_info_command, lazy_bind_off) ==
                   offsetof (::dyld_info_command, lazy_bind_off));
     STATIC_ASSERT (offsetof (dyld_info_command, lazy_bind_size) ==
                   offsetof (::dyld_info_command, lazy_bind_size));
     STATIC_ASSERT (offsetof (dyld_info_command, export_off) ==
                   offsetof (::dyld_info_command, export_off));
     STATIC_ASSERT (offsetof (dyld_info_command, export_size) ==
                   offsetof (::dyld_info_command, export_size));
#else
     STATIC_ASSERT (sizeof (dyld_info_command) == 48);
     STATIC_ASSERT (offsetof (dyld_info_command, cmd) == 0);
     STATIC_ASSERT (offsetof (dyld_info_command, cmdsize) == 4);
     STATIC_ASSERT (offsetof (dyld_info_command, rebase_off) == 8);
     STATIC_ASSERT (offsetof (dyld_info_command, rebase_size) == 12);
     STATIC_ASSERT (offsetof (dyld_info_command, bind_off) == 16);
     STATIC_ASSERT (offsetof (dyld_info_command, bind_size) == 20);
     STATIC_ASSERT (offsetof (dyld_info_command, weak_bind_off) == 24);
     STATIC_ASSERT (offsetof (dyld_info_command, weak_bind_size) == 28);
     STATIC_ASSERT (offsetof (dyld_info_command, lazy_bind_off) == 32);
     STATIC_ASSERT (offsetof (dyld_info_command, lazy_bind_size) == 36);
     STATIC_ASSERT (offsetof (dyld_info_command, export_off) == 40);
     STATIC_ASSERT (offsetof (dyld_info_command, export_size) == 44);
#endif



    // A program that uses a dynamic linker contains a dylinker_command to identify the name of the
    // dynamic linker (LC_LOAD_DYLINKER). And a dynamic linker contains a dylinker_command to
    // identify the dynamic linker (LC_ID_DYLINKER). A file can have at most one of these. This
    // struct is also used for the LC_DYLD_ENVIRONMENT load command and contains string for dyld to
    // treat like environment variable.
    struct dylinker_command {
        std::uint32_t cmd;     // LC_ID_DYLINKER, LC_LOAD_DYLINKER or LC_DYLD_ENVIRONMENT
        std::uint32_t cmdsize; // includes pathname string
        lc_str name;           // dynamic linker's path name */
    };

#ifdef CHECK
     STATIC_ASSERT (sizeof (dylinker_command) == sizeof (::dylinker_command));
     STATIC_ASSERT (offsetof (dylinker_command, cmd) == offsetof (::dylinker_command, cmd));
     STATIC_ASSERT (offsetof (dylinker_command, cmdsize) == offsetof (::dylinker_command, cmdsize));
     STATIC_ASSERT (offsetof (dylinker_command, name) == offsetof (::dylinker_command, name));
#else
     STATIC_ASSERT (sizeof (dylinker_command) == 12);
     STATIC_ASSERT (offsetof (dylinker_command, cmd) == 0);
     STATIC_ASSERT (offsetof (dylinker_command, cmdsize) == 4);
     STATIC_ASSERT (offsetof (dylinker_command, name) == 8);
#endif // CHECK


    // This is the second set of the symbolic information which is used to support the data
    // structures for the dynamically link editor.
    //
    // The original set of symbolic information in the symtab_command which contains the symbol and
    // string tables must also be present when this load command is present. When this load command
    // is present the symbol table is organized into three groups of symbols:
    // - local symbols (static and debugging symbols) - grouped by module
    // - defined external symbols - grouped by module (sorted by name if not lib)
    // - undefined external symbols (sorted by name if MH_BINDATLOAD is not set, and in order the
    // were seen by the static linker if MH_BINDATLOAD is set)
    //
    // In this load command there are offsets and counts to each of the three groups of symbols.
    //
    // This load command contains a the offsets and sizes of the following new symbolic information
    // tables:
    // - table of contents
    // - module table
    // - reference symbol table
    // - indirect symbol table
    // The first three tables above (the table of contents, module table and reference symbol table)
    // are only present if the file is a dynamically linked shared library. For executable and
    // object modules, which are files containing only one module, the information that would be in
    // these three tables is determined as follows:
    // - table of contents - the defined external symbols are sorted by name
    // - module table - the file contains only one module so everything in the file is part of the
    // module.
    // - reference symbol table - is the defined and undefined external symbols
    //
    // For dynamically linked shared library files this load command also contains offsets and sizes
    // to the pool of relocation entries for all sections separated into two groups:
    // - external relocation entries
    // - local relocation entries
    // For executable and object modules the relocation entries continue to hang off the section
    // structures.
    struct dysymtab_command {
        uint32_t cmd;     // LC_DYSYMTAB
        uint32_t cmdsize; // sizeof(struct dysymtab_command)

        // The symbols indicated by symoff and nsyms of the LC_SYMTAB load command are grouped into
        // the following three groups:
        // - local symbols (further grouped by the module they are from)
        // - defined external symbols (further grouped by the module they are from)
        // - undefined symbols
        //
        // The local symbols are used only for debugging. The dynamic binding process may have to
        // use them to indicate to the debugger the local symbols for a module that is being bound.
        //
        // The last two groups are used by the dynamic binding process to do the binding (indirectly
        // through the module table and the reference symbol table when this is a dynamically linked
        // shared library file).
        uint32_t ilocalsym; // index to local symbols
        uint32_t nlocalsym; // number of local symbols

        uint32_t iextdefsym; // index to externally defined symbols
        uint32_t nextdefsym; // number of externally defined symbols

        uint32_t iundefsym; // index to undefined symbols
        uint32_t nundefsym; // number of undefined symbols

        // For the dynamic binding process to find which module a symbol is defined in the table of
        // contents is used (analogous to the ranlib structure in an archive) which maps defined
        // external symbols to modules they are defined in. This exists only in a dynamically linked
        // shared library file. For executable and object modules the defined external symbols are
        // sorted by name and is use as the table of contents.
        uint32_t tocoff; // file offset to table of contents
        uint32_t ntoc;   // number of entries in table of contents


        // To support dynamic binding of "modules" (whole object files) the symbol table must
        // reflect the modules that the file was created from. This is done by having a module table
        // that has indexes and counts into the merged tables for each module. The module structure
        // that these two entries refer to is described below. This exists only in a dynamically
        // linked shared library file. For executable and object modules the file only contains one
        // module so everything in the file belongs to the module.
        uint32_t modtaboff; // file offset to module table
        uint32_t nmodtab;   // number of module table entries

        // To support dynamic module binding the module structure for each module indicates the
        // external references (defined and undefined) each module makes. For each module there is
        // an offset and a count into the reference symbol table for the symbols that the module
        // references. This exists only in a dynamically linked shared library file. For executable
        // and object modules the defined external symbols and the undefined external symbols
        // indicates the external references.

        uint32_t extrefsymoff; // offset to referenced symbol table
        uint32_t nextrefsyms;  // number of referenced symbol table entries

        //  The sections that contain "symbol pointers" and "routine stubs" have indexes and
        //  (implied counts based on the size of the section and fixed size of the entry) into the
        //  "indirect symbol" table for each pointer and stub. For every section of these two types
        //  the index into the indirect symbol table is stored in the section header in the field
        //  reserved1. An indirect symbol table entry is simply a 32bit index into the symbol table
        //  to the symbol that the pointer or stub is referring to. The indirect symbol table is
        //  ordered to match the entries in the section.
        uint32_t indirectsymoff; // file offset to the indirect symbol table
        uint32_t nindirectsyms;  // number of indirect symbol table entries

        // To support relocating an individual module in a library file quickly the external
        // relocation entries for each module in the library need to be accessed efficiently. Since
        // the relocation entries can't be accessed through the section headers for a library file
        // they are separated into groups of local and external entries further grouped by module.
        // In this case the presents of this load command who's extreloff, nextrel, locreloff and
        // nlocrel fields are non-zero indicates that the relocation entries of non-merged sections
        // are not referenced through the section structures (and the reloff and nreloc fields in
        // the section headers are set to zero).
        //
        // Since the relocation entries are not accessed through the section headers this requires
        // the r_address field to be something other than a section offset to identify the item to
        // be relocated. In this case r_address is set to the offset from the vmaddr of the first
        // LC_SEGMENT command. For MH_SPLIT_SEGS images r_address is set to the the offset from the
        // vmaddr of the first read-write LC_SEGMENT command.
        //
        // The relocation entries are grouped by module and the module table entries have indexes
        // and counts into them for the group of external relocation entries for that the module.
        //
        // For sections that are merged across modules there must not be any remaining external
        // relocation entries for them (for merged sections remaining relocation entries must be
        // local).
        uint32_t extreloff; // offset to external relocation entries
        uint32_t nextrel;   // number of external relocation entries

        // All the local relocation entries are grouped together (they are not grouped by their
        // module since they are only used if the object is moved from it staticly link edited
        // address).
        uint32_t locreloff; // offset to local relocation entries
        uint32_t nlocrel;   // number of local relocation entries
    };

    struct linkedit_data_command {
        std::uint32_t
            cmd; // LC_CODE_SIGNATURE, LC_SEGMENT_SPLIT_INFO, LC_FUNCTION_STARTS, LC_DATA_IN_CODE,
                 // LC_DYLIB_CODE_SIGN_DRS or LC_LINKER_OPTIMIZATION_HINT.
        std::uint32_t cmdsize;  // sizeof(struct linkedit_data_command)
        std::uint32_t dataoff;  // file offset of data in __LINKEDIT segment
        std::uint32_t datasize; // file size of data in __LINKEDIT segment
    };

#ifdef CHECK
     STATIC_ASSERT (sizeof (linkedit_data_command) == sizeof (::linkedit_data_command));
     STATIC_ASSERT (offsetof (linkedit_data_command, cmd) ==
                   offsetof (::linkedit_data_command, cmd));
     STATIC_ASSERT (offsetof (linkedit_data_command, cmdsize) ==
                   offsetof (::linkedit_data_command, cmdsize));
     STATIC_ASSERT (offsetof (linkedit_data_command, dataoff) ==
                   offsetof (::linkedit_data_command, dataoff));
     STATIC_ASSERT (offsetof (linkedit_data_command, datasize) ==
                   offsetof (::linkedit_data_command, datasize));
#else
     STATIC_ASSERT (sizeof (linkedit_data_command) == 16);
     STATIC_ASSERT (offsetof (linkedit_data_command, cmd) == 0);
     STATIC_ASSERT (offsetof (linkedit_data_command, cmdsize) == 4);
     STATIC_ASSERT (offsetof (linkedit_data_command, dataoff) == 8);
     STATIC_ASSERT (offsetof (linkedit_data_command, datasize) == 12);
#endif // CHECK


    // The build_version_command contains the min OS version on which this binary was built to run
    // for its platform. The list of known platforms and tool values following it.
    struct build_version_command {
        std::uint32_t cmd;      // LC_BUILD_VERSION
        std::uint32_t cmdsize;  // sizeof(struct build_version_command) plus ntools * sizeof(struct
                                // build_tool_version)
        std::uint32_t platform; // platform
        std::uint32_t minos;    // X.Y.Z is encoded in nibbles xxxx.yy.zz
        std::uint32_t sdk;      // X.Y.Z is encoded in nibbles xxxx.yy.zz
        std::uint32_t ntools;   // number of tool entries following this
    };

#ifdef CHECK
     STATIC_ASSERT (sizeof (build_version_command) == sizeof (::build_version_command));
     STATIC_ASSERT (offsetof (build_version_command, cmd) ==
                   offsetof (::build_version_command, cmd));
     STATIC_ASSERT (offsetof (build_version_command, cmdsize) ==
                   offsetof (::build_version_command, cmdsize));
     STATIC_ASSERT (offsetof (build_version_command, platform) ==
                   offsetof (::build_version_command, platform));
     STATIC_ASSERT (offsetof (build_version_command, minos) ==
                   offsetof (::build_version_command, minos));
     STATIC_ASSERT (offsetof (build_version_command, sdk) ==
                   offsetof (::build_version_command, sdk));
     STATIC_ASSERT (offsetof (build_version_command, ntools) ==
                   offsetof (::build_version_command, ntools));
#else
     STATIC_ASSERT (sizeof (build_version_command) == 24);
     STATIC_ASSERT (offsetof (build_version_command, cmd) == 0);
     STATIC_ASSERT (offsetof (build_version_command, cmdsize) == 4);
     STATIC_ASSERT (offsetof (build_version_command, platform) == 8);
     STATIC_ASSERT (offsetof (build_version_command, minos) == 12);
     STATIC_ASSERT (offsetof (build_version_command, sdk) == 16);
     STATIC_ASSERT (offsetof (build_version_command, ntools) == 20);
#endif // CHECK


    struct build_tool_version {
        uint32_t tool;    // enum for the tool
        uint32_t version; // version number of the tool
    };

#ifdef CHECK
     STATIC_ASSERT (sizeof (build_tool_version) == sizeof (::build_tool_version));
     STATIC_ASSERT (offsetof (build_tool_version, tool) == offsetof (::build_tool_version, tool));
     STATIC_ASSERT (offsetof (build_tool_version, version) ==
                   offsetof (::build_tool_version, version));
#else
     STATIC_ASSERT (sizeof (build_tool_version) == 8);
     STATIC_ASSERT (offsetof (build_tool_version, tool) == 0);
     STATIC_ASSERT (offsetof (build_tool_version, version) == 4);
#endif // CHECK


    // Dynamically linked shared libraries are identified by two things.  The pathname (the name of
    // the library as found for execution), and the compatibility version number.  The pathname must
    // match and the compatibility number in the user of the library must be greater than or equal
    // to the library being used.  The time stamp is used to record the time a library was built and
    // copied into user so it can be use to determined if the library used at runtime is exactly the
    // same as used to built the program.
    struct dylib {
        union lc_str name;                   // library's path name
        std::uint32_t timestamp;             // library's build time stamp
        std::uint32_t current_version;       // library's current version number
        std::uint32_t compatibility_version; // library's compatibility vers number
    };

#ifdef CHECK
     STATIC_ASSERT (sizeof (dylib) == sizeof (::dylib));
     STATIC_ASSERT (offsetof (dylib, name) == offsetof (::dylib, name));
     STATIC_ASSERT (offsetof (dylib, timestamp) == offsetof (::dylib, timestamp));
     STATIC_ASSERT (offsetof (dylib, current_version) == offsetof (::dylib, current_version));
     STATIC_ASSERT (offsetof (dylib, compatibility_version) ==
                   offsetof (::dylib, compatibility_version));
#else
     STATIC_ASSERT (sizeof (dylib) == 16);
     STATIC_ASSERT (offsetof (dylib, name) == 0);
     STATIC_ASSERT (offsetof (dylib, timestamp) == 4);
     STATIC_ASSERT (offsetof (dylib, current_version) == 8);
     STATIC_ASSERT (offsetof (dylib, compatibility_version) == 12);
#endif // CHECK



    // A dynamically linked shared library (filetype == MH_DYLIB in the mach header) contains a
    // dylib_command (cmd == LC_ID_DYLIB) to identify the library. An object that uses a dynamically
    // linked shared library also contains a dylib_command (cmd == LC_LOAD_DYLIB,
    // LC_LOAD_WEAK_DYLIB, or LC_REEXPORT_DYLIB) for each library it uses.
    struct dylib_command {
        std::uint32_t cmd;     // LC_ID_DYLIB, LC_LOAD_{,WEAK_}DYLIB, LC_REEXPORT_DYLIB
        std::uint32_t cmdsize; // includes pathname string
        struct dylib dylib;    // the library identification
    };

#ifdef CHECK
     STATIC_ASSERT (sizeof (dylib_command) == sizeof (::dylib_command));
     STATIC_ASSERT (offsetof (dylib_command, cmd) == offsetof (::dylib_command, cmd));
     STATIC_ASSERT (offsetof (dylib_command, cmdsize) == offsetof (::dylib_command, cmdsize));
     STATIC_ASSERT (offsetof (dylib_command, dylib) == offsetof (::dylib_command, dylib));
#else
     STATIC_ASSERT (sizeof (dylib_command) == 24);
     STATIC_ASSERT (offsetof (dylib_command, cmd) == 0);
     STATIC_ASSERT (offsetof (dylib_command, cmdsize) == 4);
     STATIC_ASSERT (offsetof (dylib_command, dylib) == 8);
#endif // CHECK



    // The entry_point_command is a replacement for thread_command. It is used for main executables
    // to specify the location (file offset) of main().  If -stack_size was used at link time, the
    // stacksize field will contain the stack size need for the main thread.
    struct entry_point_command {
        uint32_t cmd;       // LC_MAIN only used in MH_EXECUTE filetypes
        uint32_t cmdsize;   // 24
        uint64_t entryoff;  // file (__TEXT) offset of main()
        uint64_t stacksize; // if not zero, initial stack size
    };

#ifdef CHECK
     STATIC_ASSERT (sizeof (entry_point_command) == sizeof (::entry_point_command));
     STATIC_ASSERT (offsetof (entry_point_command, cmd) == offsetof (::entry_point_command, cmd));
     STATIC_ASSERT (offsetof (entry_point_command, cmdsize) ==
                   offsetof (::entry_point_command, cmdsize));
     STATIC_ASSERT (offsetof (entry_point_command, entryoff) ==
                   offsetof (::entry_point_command, entryoff));
     STATIC_ASSERT (offsetof (entry_point_command, stacksize) ==
                   offsetof (::entry_point_command, stacksize));
#else
     STATIC_ASSERT (sizeof (entry_point_command) == 24);
     STATIC_ASSERT (offsetof (entry_point_command, cmd) == 0);
     STATIC_ASSERT (offsetof (entry_point_command, cmdsize) == 4);
     STATIC_ASSERT (offsetof (entry_point_command, entryoff) == 8);
     STATIC_ASSERT (offsetof (entry_point_command, stacksize) == 16);
#endif // CHECK



    struct section_64 {
        section_64 (char const * sectname_, char const * segname_, std::uint64_t addr_,
                    std::uint64_t size_, std::uint32_t offset_, std::uint32_t align_,
                    std::uint32_t reloff_, std::uint32_t nreloc_, std::uint32_t flags_)
                : sectname{0}
                , segname{0}
                , addr{addr_}
                , size{size_}
                , offset{offset_}
                , align{align_}
                , reloff{reloff_}
                , nreloc{nreloc_}
                , flags{flags_}
                , reserved1{0}
                , reserved2{0}
                , reserved3{0} {

            std::strncpy (sectname, sectname_, 16);
            std::strncpy (segname, segname_, 16);
        }
        char sectname[16];       // name of this section
        char segname[16];        // segment this section goes in
        std::uint64_t addr;      // memory address of this section
        std::uint64_t size;      // size in bytes of this section
        std::uint32_t offset;    // file offset of this section
        std::uint32_t align;     // section alignment (power of 2)
        std::uint32_t reloff;    // file offset of relocation entries
        std::uint32_t nreloc;    // number of relocation entries
        std::uint32_t flags;     // flags (section type and attributes)
        std::uint32_t reserved1; // reserved (for offset or index)
        std::uint32_t reserved2; // reserved (for count or sizeof)
        std::uint32_t reserved3; // reserved
    };

#ifdef CHECK
     STATIC_ASSERT (sizeof (section_64) == sizeof (::section_64));
     STATIC_ASSERT (offsetof (section_64, sectname) == offsetof (::section_64, sectname));
     STATIC_ASSERT (offsetof (section_64, segname) == offsetof (::section_64, segname));
     STATIC_ASSERT (offsetof (section_64, addr) == offsetof (::section_64, addr));
     STATIC_ASSERT (offsetof (section_64, size) == offsetof (::section_64, size));
     STATIC_ASSERT (offsetof (section_64, offset) == offsetof (::section_64, offset));
     STATIC_ASSERT (offsetof (section_64, align) == offsetof (::section_64, align));
     STATIC_ASSERT (offsetof (section_64, reloff) == offsetof (::section_64, reloff));
     STATIC_ASSERT (offsetof (section_64, nreloc) == offsetof (::section_64, nreloc));
     STATIC_ASSERT (offsetof (section_64, flags) == offsetof (::section_64, flags));
     STATIC_ASSERT (offsetof (section_64, reserved1) == offsetof (::section_64, reserved1));
     STATIC_ASSERT (offsetof (section_64, reserved2) == offsetof (::section_64, reserved2));
     STATIC_ASSERT (offsetof (section_64, reserved3) == offsetof (::section_64, reserved3));
#else
     STATIC_ASSERT (sizeof (section_64) == 80);
     STATIC_ASSERT (offsetof (section_64, sectname) == 0);
     STATIC_ASSERT (offsetof (section_64, segname) == 16);
     STATIC_ASSERT (offsetof (section_64, addr) == 32);
     STATIC_ASSERT (offsetof (section_64, size) == 40);
     STATIC_ASSERT (offsetof (section_64, offset) == 48);
     STATIC_ASSERT (offsetof (section_64, align) == 52);
     STATIC_ASSERT (offsetof (section_64, reloff) == 56);
     STATIC_ASSERT (offsetof (section_64, nreloc) == 60);
     STATIC_ASSERT (offsetof (section_64, flags) == 64);
     STATIC_ASSERT (offsetof (section_64, reserved1) == 68);
     STATIC_ASSERT (offsetof (section_64, reserved2) == 72);
     STATIC_ASSERT (offsetof (section_64, reserved3) == 76);
#endif // CHECK



    using vm_prot_t = std::uint32_t;

    // Protection values, defined as bits within the vm_prot_t type
    enum : vm_prot_t {
        vm_prot_none = vm_prot_t{0x00},

        vm_prot_read = vm_prot_t{0x01},    // read permission
        vm_prot_write = vm_prot_t{0x02},   // write permission
        vm_prot_execute = vm_prot_t{0x04}, // execute permission

        // The maximum privileges possible, for parameter checking.
        vm_prot_all = vm_prot_read | vm_prot_write | vm_prot_execute,

    };



    // The 64-bit segment load command indicates that a part of this file is to be mapped into a
    // 64-bit task's address space.  If the 64-bit segment has sections then section_64 structures
    // directly follow the 64-bit segment command and their size is reflected in cmdsize.
    struct segment_command_64 {
        std::uint32_t cmd;      // LC_SEGMENT_64
        std::uint32_t cmdsize;  // includes sizeof section_64 structs
        char segname[16];       // segment name
        std::uint64_t vmaddr;   // memory address of this segment
        std::uint64_t vmsize;   // memory size of this segment
        std::uint64_t fileoff;  // file offset of this segment
        std::uint64_t filesize; // amount to map from the file
        vm_prot_t maxprot;      // maximum VM protection
        vm_prot_t initprot;     // initial VM protection
        std::uint32_t nsects;   // number of sections in segment
        std::uint32_t flags;    // flags
    };

#ifdef CHECK
     STATIC_ASSERT (sizeof (segment_command_64) == sizeof (::segment_command_64));
     STATIC_ASSERT (offsetof (segment_command_64, cmd) == offsetof (::segment_command_64, cmd));
     STATIC_ASSERT (offsetof (segment_command_64, cmdsize) ==
                   offsetof (::segment_command_64, cmdsize));
     STATIC_ASSERT (offsetof (segment_command_64, segname) ==
                   offsetof (::segment_command_64, segname));
     STATIC_ASSERT (offsetof (segment_command_64, vmaddr) ==
                   offsetof (::segment_command_64, vmaddr));
     STATIC_ASSERT (offsetof (segment_command_64, vmsize) ==
                   offsetof (::segment_command_64, vmsize));
     STATIC_ASSERT (offsetof (segment_command_64, fileoff) ==
                   offsetof (::segment_command_64, fileoff));
     STATIC_ASSERT (offsetof (segment_command_64, filesize) ==
                   offsetof (::segment_command_64, filesize));
     STATIC_ASSERT (offsetof (segment_command_64, maxprot) ==
                   offsetof (::segment_command_64, maxprot));
     STATIC_ASSERT (offsetof (segment_command_64, initprot) ==
                   offsetof (::segment_command_64, initprot));
     STATIC_ASSERT (offsetof (segment_command_64, nsects) ==
                   offsetof (::segment_command_64, nsects));
     STATIC_ASSERT (offsetof (segment_command_64, flags) == offsetof (::segment_command_64, flags));
#else
     STATIC_ASSERT (sizeof (segment_command_64) == 72);
     STATIC_ASSERT (offsetof (segment_command_64, cmd) == 0);
     STATIC_ASSERT (offsetof (segment_command_64, cmdsize) == 4);
     STATIC_ASSERT (offsetof (segment_command_64, segname) == 8);
     STATIC_ASSERT (offsetof (segment_command_64, vmaddr) == 24);
     STATIC_ASSERT (offsetof (segment_command_64, vmsize) == 32);
     STATIC_ASSERT (offsetof (segment_command_64, fileoff) == 40);
     STATIC_ASSERT (offsetof (segment_command_64, filesize) == 48);
     STATIC_ASSERT (offsetof (segment_command_64, maxprot) == 56);
     STATIC_ASSERT (offsetof (segment_command_64, initprot) == 60);
     STATIC_ASSERT (offsetof (segment_command_64, nsects) == 64);
     STATIC_ASSERT (offsetof (segment_command_64, flags) == 68);
#endif // CHECK



    // The symtab_command contains the offsets and sizes of the link-edit 4.3BSD "stab" style symbol
    // table information as described in the header files <nlist.h> and <stab.h>.
    struct symtab_command {
        std::uint32_t cmd;     // LC_SYMTAB
        std::uint32_t cmdsize; // sizeof(struct symtab_command)
        std::uint32_t symoff;  // symbol table offset
        std::uint32_t nsyms;   // number of symbol table entries
        std::uint32_t stroff;  // string table offset */
        std::uint32_t strsize; // string table size in bytes
    };

#ifdef CHECK
     STATIC_ASSERT (sizeof (symtab_command) == sizeof (::symtab_command));
     STATIC_ASSERT (offsetof (symtab_command, cmd) == offsetof (::symtab_command, cmd));
     STATIC_ASSERT (offsetof (symtab_command, cmdsize) == offsetof (::symtab_command, cmdsize));
     STATIC_ASSERT (offsetof (symtab_command, symoff) == offsetof (::symtab_command, symoff));
     STATIC_ASSERT (offsetof (symtab_command, nsyms) == offsetof (::symtab_command, nsyms));
     STATIC_ASSERT (offsetof (symtab_command, stroff) == offsetof (::symtab_command, stroff));
     STATIC_ASSERT (offsetof (symtab_command, strsize) == offsetof (::symtab_command, strsize));
#else
     STATIC_ASSERT (sizeof (symtab_command) == 24);
     STATIC_ASSERT (offsetof (symtab_command, cmd) == 0);
     STATIC_ASSERT (offsetof (symtab_command, cmdsize) == 4);
     STATIC_ASSERT (offsetof (symtab_command, symoff) == 8);
     STATIC_ASSERT (offsetof (symtab_command, nsyms) == 12);
     STATIC_ASSERT (offsetof (symtab_command, stroff) == 16);
     STATIC_ASSERT (offsetof (symtab_command, strsize) == 20);
#endif // CHECK



    // The uuid load command contains a single 128-bit unique random number that identifies an
    // object produced by the static link editor.
    struct uuid_command {
        std::uint32_t cmd;     // LC_UUID
        std::uint32_t cmdsize; // sizeof(struct uuid_command)
        std::uint8_t uuid[16]; // the 128-bit uuid
    };

#ifdef CHECK
     STATIC_ASSERT (sizeof (uuid_command) == sizeof (::uuid_command));
     STATIC_ASSERT (offsetof (uuid_command, cmd) == offsetof (::uuid_command, cmd));
     STATIC_ASSERT (offsetof (uuid_command, cmdsize) == offsetof (::uuid_command, cmdsize));
     STATIC_ASSERT (offsetof (uuid_command, uuid) == offsetof (::uuid_command, uuid));
#else
     STATIC_ASSERT (sizeof (uuid_command) == 24);
     STATIC_ASSERT (offsetof (uuid_command, cmd) == 0);
     STATIC_ASSERT (offsetof (uuid_command, cmdsize) == 4);
     STATIC_ASSERT (offsetof (uuid_command, uuid) == 8);
#endif // CHECK



    // After MacOS X 10.1 when a new load command is added that is required to be understood by the
    // dynamic linker for the image to execute properly the LC_REQ_DYLD bit will be or'ed into the
    // load command constant. If the dynamic linker sees such a load command it it does not
    // understand will issue a "unknown load command required for execution" error and refuse to use
    // the image. Other load commands without this bit that are not understood will simply be
    // ignored.
    constexpr std::uint32_t lc_req_dyld = 0x80000000;

    // Constants for the cmd field of all load commands
    enum : std::uint32_t {
        lc_segment = 0x1,    // segment of this file to be mapped
        lc_symtab = 0x2,     // link-edit stab symbol table info
        lc_symseg = 0x3,     // link-edit gdb symbol table info (obsolete)
        lc_thread = 0x4,     // thread
        lc_unixthread = 0x5, // unix thread (includes a stack) */
        //#define LC_LOADFVMLIB    0x6    // load a specified fixed VM shared library
        //#define LC_IDFVMLIB    0x7    // fixed VM shared library identification
        lc_ident = 0x8, // object identification info (obsolete)
        //#define LC_FVMFILE    0x9    // fixed VM file inclusion (internal use)
        //#define LC_PREPAGE      0xa     // prepage command (internal use)
        lc_dysymtab = 0xb,   // dynamic link-edit symbol table info
        lc_load_dylib = 0xc, // load a dynamically linked shared library
        //#define LC_ID_DYLIB    0xd    // dynamically linked shared lib ident
        lc_load_dylinker = 0xe,   // load a dynamic linker
        lc_id_dylinker = 0xf,     // dynamic linker identification
        lc_prebound_dylib = 0x10, // modules prebound for a dynamically linked shared library

        lc_routines = 0x11,      // image routines
        lc_sub_framework = 0x12, // sub framework
        lc_sub_umbrella = 0x13,  // sub umbrella
        // #define LC_SUB_CLIENT 0x14 // sub client
        // #define LC_SUB_LIBRARY  0x15    // sub library
        // #define LC_TWOLEVEL_HINTS 0x16    // two-level namespace lookup hints
        lc_prebind_cksum = 0x17, // prebind checksum
        // #define LC_LOAD_WEAK_DYLIB (0x18 | LC_REQ_DYLD)
        lc_segment_64 = 0x19,            // 64-bit segment of this file to be mapped
        lc_routines_64 = 0x1a,           // 64-bit image routines
        lc_uuid = 0x1b,                  // the uuid
        lc_rpath = (0x1c | lc_req_dyld), // runpath additions
        lc_code_signature = 0x1d,        // local of code signature
        // #define LC_SEGMENT_SPLIT_INFO 0x1e // local of info to split segments
        // #define LC_REEXPORT_DYLIB (0x1f | LC_REQ_DYLD) // load and re-export dylib
        // #define LC_LAZY_LOAD_DYLIB 0x20    // delay load of dylib until first use
        // #define LC_ENCRYPTION_INFO 0x21    // encrypted segment information
        lc_dyld_info = 0x22,                       //  compressed dyld information
        lc_dyld_info_only = 0x22 | lc_req_dyld,    // compressed dyld information only
        lc_load_upward_dylib = 0x23 | lc_req_dyld, // load upward dylib
        lc_version_min_macosx = 0x24,              // build for MacOSX min OS version
        //#define LC_VERSION_MIN_IPHONEOS 0x25 // build for iPhoneOS min OS version
        //#define LC_FUNCTION_STARTS 0x26 // compressed table of function start addresses
        //#define LC_DYLD_ENVIRONMENT 0x27 // string for dyld to treat like environment variable
        lc_main = 0x28 | lc_req_dyld, // replacement for LC_UNIXTHREAD
        lc_data_in_code = 0x29,       // table of non-instructions in __text
        //#define LC_SOURCE_VERSION 0x2A // source version used to build binary
        lc_dylib_code_sign_drs = 0x2B, // Code signing DRs copied from linked dylibs
        //#define LC_ENCRYPTION_INFO_64 0x2C // 64-bit encrypted segment information
        lc_linker_option = 0x2D, // linker options in MH_OBJECT files
        //#define LC_LINKER_OPTIMIZATION_HINT 0x2E // optimization hints in MH_OBJECT files
        //#define LC_VERSION_MIN_TVOS 0x2F // build for AppleTV min OS version
        //#define LC_VERSION_MIN_WATCHOS 0x30 // build for Watch min OS version
        lc_note = 0x31,          // arbitrary data included within a Mach-O file
        lc_build_version = 0x32, // build for platform min OS version
        lc_dyld_exports_trie =
            (0x33 | lc_req_dyld), // used with linkedit_data_command, payload is trie
        lc_dyld_chained_fixups = (0x34 | lc_req_dyld), // used with linkedit_data_command
    };

    constexpr std::uint32_t platform_macos = 1;
    constexpr std::uint32_t tool_ld = 3;

#ifdef CHECK
     STATIC_ASSERT (lc_segment == LC_SEGMENT);
     STATIC_ASSERT (lc_symtab == LC_SYMTAB);
     STATIC_ASSERT (lc_symseg == LC_SYMSEG);
     STATIC_ASSERT (lc_thread == LC_THREAD);
     STATIC_ASSERT (lc_unixthread == LC_UNIXTHREAD);
    //  STATIC_ASSERT (LC_LOADFVMLIB    0x6    // load a specified fixed VM shared library
    //  STATIC_ASSERT (LC_IDFVMLIB    0x7    // fixed VM shared library identification
    //  STATIC_ASSERT (LC_IDENT    0x8    // object identification info (obsolete)
    //  STATIC_ASSERT (LC_FVMFILE    0x9    // fixed VM file inclusion (internal use)
    //  STATIC_ASSERT (LC_PREPAGE      0xa     // prepage command (internal use)
    //  STATIC_ASSERT (LC_DYSYMTAB    0xb    // dynamic link-edit symbol table info
     STATIC_ASSERT (lc_load_dylib == LC_LOAD_DYLIB);
    //  STATIC_ASSERT (LC_ID_DYLIB    0xd    // dynamically linked shared lib ident
    //  STATIC_ASSERT (LC_LOAD_DYLINKER 0xe    // load a dynamic linker
    //  STATIC_ASSERT (LC_ID_DYLINKER    0xf    // dynamic linker identification
     STATIC_ASSERT (lc_prebound_dylib == LC_PREBOUND_DYLIB);
     STATIC_ASSERT (lc_routines == LC_ROUTINES);
     STATIC_ASSERT (lc_sub_framework == LC_SUB_FRAMEWORK);
    // umbrella  STATIC_ASSERT (LC_SUB_CLIENT    0x14    // sub client
    // STATIC_ASSERT (LC_SUB_LIBRARY 0x15    // sub library
    // STATIC_ASSERT (LC_TWOLEVEL_HINTS 0x16    // two-level namespace lookup hints
     STATIC_ASSERT (lc_prebind_cksum == LC_PREBIND_CKSUM);
     STATIC_ASSERT (lc_segment_64 == LC_SEGMENT_64);
     STATIC_ASSERT (lc_routines_64 == LC_ROUTINES_64);
     STATIC_ASSERT (lc_uuid == LC_UUID);
     STATIC_ASSERT (lc_rpath == LC_RPATH);
     STATIC_ASSERT (lc_code_signature == LC_CODE_SIGNATURE);
    //  STATIC_ASSERT (LC_SEGMENT_SPLIT_INFO 0x1e // local of info to split segments
    //  STATIC_ASSERT (LC_REEXPORT_DYLIB (0x1f | LC_REQ_DYLD) // load and re-export dylib
    //  STATIC_ASSERT (LC_LAZY_LOAD_DYLIB 0x20    // delay load of dylib until first use
    //  STATIC_ASSERT (LC_ENCRYPTION_INFO 0x21    // encrypted segment information
     STATIC_ASSERT (lc_dyld_info == LC_DYLD_INFO);
    //  STATIC_ASSERT (LC_DYLD_INFO_ONLY (0x22|LC_REQ_DYLD)    // compressed dyld information only
    //  STATIC_ASSERT (LC_LOAD_UPWARD_DYLIB (0x23 | LC_REQ_DYLD) // load upward dylib
    //  STATIC_ASSERT (LC_VERSION_MIN_MACOSX 0x24   // build for MacOSX min OS version
    //  STATIC_ASSERT (LC_VERSION_MIN_IPHONEOS 0x25 // build for iPhoneOS min OS version
    //  STATIC_ASSERT (LC_FUNCTION_STARTS 0x26 // compressed table of function start addresses
    //  STATIC_ASSERT (LC_DYLD_ENVIRONMENT 0x27 // string for dyld to treat like environment variable
     STATIC_ASSERT (lc_main == LC_MAIN);
     STATIC_ASSERT (lc_data_in_code == LC_DATA_IN_CODE);
    //  STATIC_ASSERT (LC_SOURCE_VERSION 0x2A // source version used to build binary
    //  STATIC_ASSERT (LC_DYLIB_CODE_SIGN_DRS 0x2B // Code signing DRs copied from linked dylibs
    //  STATIC_ASSERT (LC_ENCRYPTION_INFO_64 0x2C // 64-bit encrypted segment information
    //  STATIC_ASSERT (LC_LINKER_OPTION 0x2D // linker options in MH_OBJECT files
    //  STATIC_ASSERT (LC_LINKER_OPTIMIZATION_HINT 0x2E // optimization hints in MH_OBJECT files
    //  STATIC_ASSERT (LC_VERSION_MIN_TVOS 0x2F // build for AppleTV min OS version
    //  STATIC_ASSERT (LC_VERSION_MIN_WATCHOS 0x30 // build for Watch min OS version
     STATIC_ASSERT (lc_note == LC_NOTE);
     STATIC_ASSERT (lc_build_version == LC_BUILD_VERSION);
     STATIC_ASSERT (lc_dyld_exports_trie == LC_DYLD_EXPORTS_TRIE);
     STATIC_ASSERT (lc_dyld_chained_fixups == LC_DYLD_CHAINED_FIXUPS);

     STATIC_ASSERT (platform_macos == PLATFORM_MACOS);
     STATIC_ASSERT (tool_ld == TOOL_LD);
#endif // CHECK



    /*
     * Constants for the section attributes part of the flags field of a section
     * structure.
     */
    constexpr std::uint32_t section_attributes_usr = 0xff000000; // User setable attributes
    enum : std::uint32_t {
        s_attr_pure_instructions = 0x80000000, // section contains only true machine instructions
        s_attr_no_toc = 0x40000000, // section contains coalesced symbols that are not to be in a
                                    // ranlib table of contents
        s_attr_strip_static_syms = 0x20000000,   // ok to strip static symbols in this section in
                                                 // files with the MH_DYLDLINK flag
        s_attr_no_dead_strip = 0x10000000,       // no dead stripping
        s_attr_live_support = 0x08000000,        // blocks are live if they reference live blocks
        s_attr_self_modifying_code = 0x04000000, // Used with i386 code stubs written on by dyld
        s_attr_some_instructions = 0x00000400,   // section contains some machine instructions

    };

    // Constants for the type of a section
    enum {
        s_regular = 0x0,          // regular section
        s_zerofill = 0x1,         // zero fill on demand section
        s_cstring_literals = 0x2, // section with only literal C strings
        s_4byte_literals = 0x3,   // section with only 4 byte literals
        s_8byte_literals = 0x4,   // section with only 8 byte literals
        s_literal_pointers = 0x5, // section with only pointers to literals
    };



    constexpr auto seg_pagezero = "__PAGEZERO"; // the pagezero segment which has no protections and
                                                // catches NULL references for MH_EXECUTE files.
    constexpr auto seg_text = "__TEXT";         // the traditional UNIX text segment
    constexpr auto sect_text =
        "__text"; // the real text part of the text section: no headers, and no padding
    constexpr auto sect_fvmlib_init0 = "__fvmlib_init0"; // the fvmlib initialization section
    constexpr auto sect_fvmlib_init1 =
        "__fvmlib_init1"; // the section following the fvmlib initialization section

    constexpr auto seg_data = "__DATA"; // the traditional UNIX data segment
    constexpr auto sect_data =
        "__data"; // the real initialized data section: no padding, no bss overlap
    constexpr auto sect_bss = "__bss"; // the real uninitialized data section: no padding
    constexpr auto sect_common =
        "__common"; // the section to which common symbols are allocated by the link editor

    constexpr auto seg_objc = "__OBJC";                   // objective-C runtime segment
    constexpr auto sect_objc_symbols = "__symbol_table";  // symbol table
    constexpr auto sect_objc_modules = "__module_info";   // module information
    constexpr auto sect_objc_strings = "__selector_strs"; // string table
    constexpr auto sect_objc_refs = "__selector_refs";    // string table

    constexpr auto seg_icon = "__ICON";           // the icon segment
    constexpr auto sect_icon_header = "__header"; // the icon headers
    constexpr auto sect_icon_tiff = "__tiff";     // the icons in tiff format

    constexpr auto seg_linkedit =
        "__LINKEDIT"; // the segment containing all structs created and maintained by the link
                      // editor.  Created with -seglinkedit option to ld(1) for MH_EXECUTE and
                      // FVMLIB file types only.

    constexpr auto seg_unixstack = "__UNIXSTACK"; // the unix stack segment

    constexpr auto seg_import = "__IMPORT"; // the segment for the self (dyld) modifing code stubs
                                            // that has read, write and execute permissions

} // end namespace mach_o

#endif // MACH_O_HPP
