#ifndef LC_MAIN_HPP
#define LC_MAIN_HPP

#include "command.hpp"
#include "lc_segment.hpp"
#include "util.hpp"

class lc_main : public command {
public:
    explicit lc_main (not_null<lc_segment::section_value const *> main) noexcept;
    std::uint32_t size_bytes () const noexcept override;
    std::uint64_t write_command (int fd, std::uint64_t offset) override;

private:
    not_null<lc_segment::section_value const *> main_;
};

#endif // LC_MAIN_HPP
