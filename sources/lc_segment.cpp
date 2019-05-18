#include "lc_segment.hpp"

#include <numeric>
#include <unistd.h>

namespace {

    constexpr std::uint64_t page_size = 0x1000;
    inline constexpr std::uint64_t aligned (std::uint64_t v) noexcept {
        return v + calc_alignment (v, page_size);
    }

} // namespace

// ctor
// ~~~~
lc_segment::lc_segment (char const * segname, position vm, vm_prot_t maxprot, vm_prot_t initprot,
                        std::uint32_t flags) noexcept {
    v_.cmd = LC_SEGMENT_64;
    v_.cmdsize = 0; // includes sizeof section_64 structs (patched up later)
    std::strncpy (v_.segname, segname, array_elements (v_.segname)); // segment name
    v_.vmaddr = vm.first;   // memory address of this segment
    v_.vmsize = vm.second;  // memory size of this segment (patched up later)
    v_.fileoff = 0;         // file offset of this segment (patchd up later)
    v_.filesize = 0;        // amount to map from the file (patched up later)
    v_.maxprot = maxprot;   // maximum VM protection
    v_.initprot = initprot; // initial VM protection
    v_.nsects = 0;          // number of sections in segment (patched up later)
    v_.flags = flags;       // flags
}

// add_section
// ~~~~~~~~~~~
auto lc_segment::add_section (section_64 const & sec, contents_range const & contents)
    -> section_value & {
    static_assert (sizeof (v_.segname) == sizeof (sec.segname));
    assert (std::strncmp (sec.segname, v_.segname, array_elements (v_.segname)) == 0);
    sections_.emplace_back (sec, contents);
    return sections_.back ();
}

// size_bytes
// ~~~~~~~~~~
std::uint32_t lc_segment::size_bytes () const noexcept {
    auto const resl = sizeof (v_) + sections_.size () * sizeof (section_64);
    assert (resl % 8 == 0);
    return narrow_cast<std::uint32_t> (resl);
}

// payload_size
// ~~~~~~~~~~~~
std::size_t lc_segment::payload_size () const noexcept {
    return std::accumulate (
        std::begin (sections_), std::end (sections_), std::size_t{0},
        [](std::size_t acc, section_value const & sv) noexcept {
            return acc + sv.contents_size ();
        });
}

// write_command
// ~~~~~~~~~~~~~
std::uint64_t lc_segment::write_command (int fd, std::uint64_t payload_offset) {
    std::uint64_t const file_off = this->file_offset (payload_offset);
    std::uint64_t const size = this->payload_size ();

    v_.cmdsize = this->size_bytes ();
    v_.nsects = narrow_cast<decltype (v_.nsects)> (sections_.size ());
    v_.filesize = sections_.empty () ? uint64_t{0} : size + payload_offset - file_off;
    v_.fileoff = aligned (v_.filesize == 0 ? uint64_t{0} : file_off);
    v_.vmsize = aligned (std::max ({v_.vmsize, size, v_.filesize}));

    ::write (fd, &v_, sizeof (v_));

    if (sections_.empty ()) {
        return payload_offset;
    }

    payload_pos_ = v_.fileoff;
    std::uint64_t vm_addr = v_.vmaddr + (payload_offset - file_off);
    for (section_value & sv : sections_) {
        section_64 & section = sv.get ();
        section.addr = vm_addr;
        section.offset = narrow_cast<decltype (section.offset)> (payload_offset);
        section.size = sv.contents_size ();
        ::write (fd, &section, sizeof (section));

        sv.set_offset (payload_offset);

        payload_offset += section.size;
        vm_addr += section.size;
    }

    return aligned (v_.fileoff + v_.filesize);
}

// write_payload
// ~~~~~~~~~~~~~
void lc_segment::write_payload (int fd) {
    if (sections_.empty ()) {
        return;
    }

    if (std::uint64_t const x = payload_pos_.value ()) {
        lseek (fd, static_cast<off_t> (x), SEEK_SET);
    }
    for (auto const & sv : sections_) {
        ::write (fd, sv.contents ().first, sv.contents_size ());
    }
}

// file_offset
// ~~~~~~~~~~~
std::uint64_t lc_segment::file_offset (std::uint64_t offset) const noexcept {
    return offset;
}



void lc_segment::section_value::set_offset (std::uint64_t offset) noexcept {
    offset_ = offset;
}

std::uint64_t lc_segment::section_value::get_offset () const noexcept {
    assert (offset_.has_value ());
    auto const resl = offset_.value ();
    assert (resl >= 0);
    return resl;
}

std::size_t lc_segment::section_value::contents_size () const noexcept {
    auto const resl = std::distance (static_cast<std::uint8_t const *> (contents_.first),
                                     static_cast<std::uint8_t const *> (contents_.second));
    assert (resl >= 0);
    return narrow_cast<std::size_t> (static_cast<std::make_unsigned_t<decltype (resl)>> (resl));
}



std::uint64_t lc_text_segment::file_offset (std::uint64_t /*offset*/) const noexcept {
    return 0;
}
