#ifndef LC_UUID_HPP
#define LC_UUID_HPP

#include "command.hpp"

class lc_uuid : public command {
public:
    std::uint32_t size_bytes () const noexcept override;
    std::uint64_t write_command (int fd, std::uint64_t offset) override;
};

#endif // LC_UUID_HPP
