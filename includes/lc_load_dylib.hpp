#ifndef LC_LOAD_DYLIB_HPP
#define LC_LOAD_DYLIB_HPP

#include <string>
#include "command.hpp"

class lc_load_dylib : public command {
public:
    explicit lc_load_dylib (std::string name)
            : name_{std::move (name)} {}
    std::uint32_t size_bytes () const noexcept override;
    std::uint64_t write_command (int fd, std::uint64_t offset) override;

private:
    std::string name_;
};

#endif // LC_LOAD_DYLIB_HPP
