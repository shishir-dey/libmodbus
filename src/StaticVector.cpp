
#include <StaticVector.hpp>

template <typename T, size_t N>
StaticVector<T, N>::StaticVector()
    : pool(buffer.data(), buffer.size())
    , vec(&pool)
{
}

template <typename T, size_t N>
std::pmr::vector<T> StaticVector<T, N>::createVector()
{
    return vec;
}