#ifndef LC_DYLD_INFO_ONLY_HPP
#define LC_DYLD_INFO_ONLY_HPP

#include "command.hpp"

class lc_dyld_info_only : public command {
public:
    std::uint32_t size_bytes () const noexcept override;
    std::uint64_t write_command (int fd, std::uint64_t offset) override;
};

#endif // LC_DYLD_INFO_ONLY_HPP
