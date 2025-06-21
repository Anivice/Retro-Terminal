#include "file.h"
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>

void file::close() const
{
    if (fd > 0)
    {
        ::close(fd);
    }
}

void file::open(const std::string & path)
{
    fd = ::open(path.c_str(), O_RDONLY);
    if (fd < 0)
    {
        throw std::runtime_error("file::open():" + std::string(strerror(errno)));
    }
}

size_t file::read(void * buffer, const size_t location, const size_t size) const
{
    if (::lseek(fd, static_cast<long>(location), SEEK_SET) == -1)
    {
        throw std::runtime_error("file::read():" + std::string(strerror(errno)));
    }
    return ::read(fd, buffer, size);
}

size_t file::write(const void * buffer, const size_t location, const size_t size)
{
    if (::lseek(fd, static_cast<long>(location), SEEK_SET) == -1)
    {
        throw std::runtime_error("file::read():" + std::string(strerror(errno)));
    }
    return ::write(fd, buffer, size);
}

file::~file()
{
    close();
}

file::file(const file& other)
{
    close();
    fd = other.fd;
}

file::file(file&& other)  noexcept
{
    fd = other.fd;
}

file& file::operator=(file&& other) noexcept
{
    fd = other.fd; return *this;
}
