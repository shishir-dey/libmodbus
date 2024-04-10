#ifndef STATIC_VECTOR_HPP
#define STATIC_VECTOR_HPP

#include <array>
#include <cstdint>
#include <memory_resource>
#include <vector>

template <typename T, size_t N>
class StaticVector {
    std::array<std::byte, N * sizeof(T)> buffer;
    std::pmr::monotonic_buffer_resource pool;
    std::pmr::vector<T> vec;

public:
    StaticVector();
    std::pmr::vector<T> createVector();
};

#endif // STATIC_VECTOR_HPP
