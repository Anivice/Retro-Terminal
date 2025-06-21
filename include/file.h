#ifndef FILE_H
#define FILE_H

#include <string>

class file
{
private:
    int fd = -1;

public:
    void close() const;
    void open(const std::string & path);
    size_t read(void * buffer, size_t location, size_t size) const;
    size_t write(const void * buffer, size_t location, size_t size);

    ~file();
    file() = default;
    file(const file& other);
    file(file&& other) noexcept;
    file& operator=(const file& other) = default;
    file& operator=(file&& other) noexcept;
    [[nodiscard]] int get_fd() const { return fd; }
};

#endif //FILE_H
