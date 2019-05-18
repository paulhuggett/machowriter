#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <cstdint>
#include <cstdlib>
#include <sys/types.h>

class command {
public:
    virtual ~command () noexcept = default;
    virtual std::uint32_t size_bytes () const noexcept = 0;
    virtual std::uint64_t write_command (int fd, std::uint64_t offset) = 0;
    virtual void write_payload (int fd);
};

#endif // COMMAND_HPP
