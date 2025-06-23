#ifndef FILE_H
#define FILE_H

#include <cstdint>
#include <string>

class file
{
private:
    int fd = -1;

public:
    void close() const;
    void open(const std::string & path, int mode);
    void open_rw(const std::string & path);
    void read(void * buffer, size_t location, size_t size) const;
    void write(const void * buffer, size_t location, size_t size);
    explicit file(const std::string & path, const int mode) { open(path, mode); }
    explicit file(const std::string & path) { open_rw(path); }
    void flush() const;
    [[nodiscard]] uint64_t get_size() const;

    ~file();
    file() = default;
    file(const file& other);
    file(file&& other) noexcept;
    file& operator=(const file& other) = default;
    file& operator=(file&& other) noexcept;
    [[nodiscard]] int get_fd() const { return fd; }
};

#endif //FILE_H
