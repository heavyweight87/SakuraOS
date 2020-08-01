#pragma once

#include <cstdint>

namespace VirtualFS {

class VirtualFile {
    public:
        virtual bool open() = 0;
        virtual std::size_t read(void *buffer, std::size_t length) = 0;
        virtual std::size_t write(void *buffer, std::size_t length) = 0;

};

}