#pragma once

#include "buffers.hpp"

namespace glwx {
class IndexAccessor {
public:
    struct Proxy {
        size_t operator=(size_t index);
        operator size_t() const;
        size_t get() const;

        uint8_t* data;
        size_t elementSize;
    };

    IndexAccessor(IndexBuffer& buffer);

    Proxy operator[](size_t index);

private:
    IndexBuffer& buffer_;
};
}
