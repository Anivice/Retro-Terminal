#include "file.h"
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include "cpp_assert.h"
#include <cerrno>
#include <sys/stat.h>

void file::close() const
{
    if (fd > 0)
    {
        ::close(fd);
    }
}

void file::open(const std::string & path, const int mode)
{
    fd = ::open(path.c_str(), mode);
    if (fd < 0)
    {
        throw runtime_error("file::open(): " + std::string(strerror(errno)));
    }
}

void file::open_rw(const std::string & path)
{
    open(path, O_RDWR);
}

void file::read(void * buffer, const size_t location, const size_t size) const
{
    if (::lseek(fd, static_cast<long>(location), SEEK_SET) == -1)
    {
        throw runtime_error("file::read(): " + std::string(strerror(errno)));
    }

    assert_throw(::read(fd, buffer, size) == size, std::string("file::read(): " + std::string(strerror(errno))));
}

void file::write(const void * buffer, const size_t location, const size_t size)
{
    if (::lseek(fd, static_cast<long>(location), SEEK_SET) == -1)
    {
        throw runtime_error("file::read(): " + std::string(strerror(errno)));
    }
    assert_throw(::write(fd, buffer, size) == size, std::string("file::write(): " + std::string(strerror(errno))));
}

void file::flush() const
{
    ::fsync(fd);
}

uint64_t file::get_size() const
{
    struct stat st{};
    if (::fstat(fd, &st) == -1)
    {
        throw runtime_error("file::get_size(): " + std::string(strerror(errno)));
    }
    return st.st_size;
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
