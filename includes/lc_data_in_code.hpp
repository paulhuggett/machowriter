#ifndef LC_DATA_IN_CODE_HPP
#define LC_DATA_IN_CODE_HPP

#include "command.hpp"

class lc_data_in_code : public command {
public:
    std::uint32_t size_bytes () const noexcept override;
    std::uint64_t write_command (int fd, std::uint64_t offset) override;
};

#endif // LC_DATA_IN_CODE_HPP
