#ifndef LC_LOAD_DYLINKER_HPP
#define LC_LOAD_DYLINKER_HPP

#include "command.hpp"

class lc_load_dylinker : public command {
public:
    std::uint32_t size_bytes () const noexcept override;
    std::uint64_t write_command (int fd, std::uint64_t offset) override;
};

#endif // LC_LOAD_DYLINKER_HPP
