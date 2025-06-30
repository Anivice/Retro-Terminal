#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <vector>
#include <string>
#include <sys/stat.h>
#include <cstdint>

#define BLOCK_SIZE (1024 * 64) /* 64KB blocks */

class directory_t {
public:
    using entry_t = std::string;                        // file name
    using stat_t = struct stat;                         // file stats
    using page_t = std::vector < uint64_t >;            // file hash pages
    using block_pointers_t = std::vector < uint64_t >;  // block pointers
    using block_t = std::array <char, BLOCK_SIZE>;

    struct file_t
    {
        entry_t entry;
        stat_t stat;
        page_t pages;
    };

    static entry_t path_to_entry(const std::string& path);
};

#endif //DIRECTORY_H
