#include <filesystem>
#include <fstream>
#include "file_access.h"
#include "core/crc64sum.h"
#include "core/g_global_config_t.h"
#include "helper/log.h"
#include "helper/lz4frame.h"

bool if_exists(const std::string & hashed_block_name)
{
    const auto my_own_directory = g_global_config.get<std::string>("server.dictionary");
    const std::string path = my_own_directory + "/" += hashed_block_name;
    return std::filesystem::exists(path);
}

directory_t::block_t get_block_on_my_end(const std::string & hashed_block_name)
{
    // 1. check if I have this block
    const auto my_own_directory = g_global_config.get<std::string>("server.dictionary");
    if (const std::string destination = my_own_directory + "/" += hashed_block_name;
        std::filesystem::exists(destination))
    {
        std::array <char, BLOCK_SIZE> in{};
        std::array <char, BLOCK_SIZE> out{};

        std::ifstream ifs(destination, std::ios::binary);
        assert_short(ifs.good());

        LZ4F_decompressionContext_t dctx;
        assert_short(!LZ4F_isError(LZ4F_createDecompressionContext(&dctx, LZ4F_VERSION)));

        size_t inPos = 0, inSize = 0, wr_off = 0;
        for (;;)
        {
            if (inPos == inSize) {                       /* need more input */
                ifs.read(in.data(), BLOCK_SIZE);
                inSize = ifs.gcount();
                inPos  = 0;
                if (inSize == 0) break;                  /* EOF */
            }

            size_t srcSize = inSize - inPos;
            size_t dstSize = BLOCK_SIZE;
            const size_t ret = LZ4F_decompress(dctx, out.data(), &dstSize,
                                         in.data() + inPos, &srcSize, nullptr);
            assert_short(!LZ4F_isError(ret));

            std::memcpy(in.data() + wr_off, out.data(), dstSize);
            wr_off += dstSize;
            inPos += srcSize;                            /* consume input */

            if (ret == 0 && inPos == inSize) break;      /* frame ended */
        }

        LZ4F_freeDecompressionContext(dctx);
        assert_short(wr_off == BLOCK_SIZE);
        return out;
    }

    throw no_such_block();
}

std::string write_block_on_my_end(const directory_t::block_t & block)
{
    CRC64 checksum;
    checksum.update(reinterpret_cast<const uint8_t *>(block.data()), block.size());
    std::string         name = checksum.get_checksum_str();
    const size_t        max = LZ4F_compressBound(BLOCK_SIZE, nullptr);   /* worst-case */
    std::vector<char>   out; out.resize(max);
    const auto          my_own_directories = g_global_config.get<std::string>("server.dictionary");
    std::ofstream       ofs(my_own_directories + "/" + name, std::ios::binary);
    assert_short(ofs.good());

    LZ4F_compressionContext_t cctx;
    assert_short(!LZ4F_isError(LZ4F_createCompressionContext(&cctx, LZ4F_VERSION)));
    size_t n = LZ4F_compressBegin(cctx, out.data(), max, nullptr);
    ofs.write(out.data(), static_cast<int>(n));
    const size_t outSize = LZ4F_compressUpdate(cctx, out.data(), max, block.data(), block.size(), nullptr);
    assert_short(!LZ4F_isError(outSize));
    ofs.write(out.data(), static_cast<int>(outSize));
    n = LZ4F_compressEnd(cctx, out.data(), max, nullptr);             /* adds footer */
    ofs.write(out.data(), static_cast<int>(n));
    LZ4F_freeCompressionContext(cctx);

    return name;
}
