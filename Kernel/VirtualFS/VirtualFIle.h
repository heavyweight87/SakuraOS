#pragma once

#include <cstdint>

namespace VirtualFS {

class VirtualFile {
    public:
    virtual std::size_t read(std::uint8_t *buffer, std::size_t length) = 0;

};

}