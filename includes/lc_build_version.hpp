#ifndef LC_BUILD_VERSION_HPP
#define LC_BUILD_VERSION_HPP

#include "command.hpp"

class lc_build_version : public command {
public:
    std::uint32_t size_bytes () const noexcept override;
    std::uint64_t write_command (int fd, std::uint64_t offset) override;
};

#endif // LC_BUILD_VERSION_HPP
