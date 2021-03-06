#ifndef LC_SEGMENT_HPP
#define LC_SEGMENT_HPP

#include <cstdlib>
#include <utility>
#include <vector>

#include "command.hpp"
#include "mach-o.hpp"
#include "util.hpp"


using position = std::pair<std::uint64_t, std::uint64_t>;

class lc_segment : public command {
public:
    using contents_range = std::pair<void const *, void const *>;

    class section_value {
    public:
        explicit section_value (mach_o::section_64 const & s, contents_range contents)
                : s_{s}
                , contents_{std::move (contents)} {}

        mach_o::section_64 const & get () const noexcept { return s_; }
        mach_o::section_64 & get () noexcept { return s_; }

        void set_offset (std::uint64_t offset) noexcept;
        std::uint64_t get_offset () const noexcept;

        contents_range const & contents () const noexcept { return contents_; }
        std::size_t contents_size () const noexcept;

    private:
        mach_o::section_64 s_;
        contents_range contents_;
        // TODO: eliminate and use std::optional<std::uint64_t> offset_.
        bool has_offset_ = false;
        std::uint64_t offset_ = 0;
    };

    explicit lc_segment (mach_o::segment_command_64 const & v) noexcept
            : v_{v} {}

    /// \param segname  Segment name.
    /// \param vm  Memory address and size of this segment.
    /// \param maxprot  Maximum VM protection.
    /// \param initprot  Initial VM protection.
    /// \param flags  Flags.
    lc_segment (char const * segname, position vm, mach_o::vm_prot_t maxprot,
                mach_o::vm_prot_t initprot, std::uint32_t flags) noexcept;

    section_value & add_section (mach_o::section_64 const & sec, contents_range const & contents);

    std::uint32_t size_bytes () const noexcept override;
    std::uint64_t write_command (int fd, std::uint64_t payload_offset) override;
    void write_payload (int fd) override;

    section_value & operator[] (std::size_t pos) noexcept { return sections_[pos]; }
    section_value const & operator[] (std::size_t pos) const noexcept { return sections_[pos]; }

protected:
    virtual std::uint64_t file_offset (std::uint64_t offset) const noexcept;

private:
    std::size_t payload_size () const noexcept;

    mach_o::segment_command_64 v_;
    // TODO: eliminate and use std::optional<std::uint64_t> payload_pos_.
    bool have_payload_pos_ = false;
    std::uint64_t payload_pos_ = 0;
    std::vector<section_value> sections_;
};

class lc_text_segment : public lc_segment {
public:
    explicit lc_text_segment (mach_o::segment_command_64 const & v) noexcept
            : lc_segment{v} {}

    lc_text_segment (char const * segname, position vm, mach_o::vm_prot_t maxprot,
                     mach_o::vm_prot_t initprot, std::uint32_t flags) noexcept
            : lc_segment (segname, vm, maxprot, initprot, flags) {}

protected:
    std::uint64_t file_offset (std::uint64_t offset) const noexcept override;
};

#endif // LC_SEGMENT_HPP
