#ifndef COMMINTERFACE_HPP
#define COMMINTERFACE_HPP

#include <array>
#include <cstddef>
#include <memory>

class CommInterface {
public:
    virtual ~CommInterface()
    {
    }

    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual std::size_t read(char* buffer, std::size_t max_size) = 0;
    virtual bool write(const char* buffer, std::size_t length) = 0;

private:
    CommInterface(const CommInterface&) = delete;
    CommInterface& operator=(const CommInterface&) = delete;
};

#endif // COMMINTERFACE_HPP
