#ifndef LC_DYSYMTAB_HPP
#define LC_DYSYMTAB_HPP

#include "command.hpp"

class lc_dysymtab : public command {
public:
    std::uint32_t size_bytes () const noexcept override;
    std::uint64_t write_command (int fd, std::uint64_t offset) override;
};

#endif // LC_DYSYMTAB_HPP
