#ifndef LC_SYMTAB_HPP
#define LC_SYMTAB_HPP

#include "command.hpp"

class lc_symtab : public command {
public:
    std::uint32_t size_bytes () const noexcept override;
    std::uint64_t write_command (int fd, std::uint64_t offset) override;
};

#endif // LC_SYMTAB_HPP
