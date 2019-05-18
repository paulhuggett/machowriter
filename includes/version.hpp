#ifndef VERSION_HPP
#define VERSION_HPP

#include <cstdint>

inline constexpr std::uint32_t version (std::uint16_t major, std::uint8_t minor,
                                        std::uint8_t revision) {
    return (static_cast<std::uint32_t> (major) << 16) | (static_cast<std::uint32_t> (minor) << 8) |
           static_cast<std::uint32_t> (revision);
}

#endif // VERSION_HPP
