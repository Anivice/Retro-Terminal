#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper/lz4frame.h"

#define CHUNK 65536

int main(void)
{
    unsigned char in [CHUNK];
    unsigned char out[CHUNK];

    LZ4F_decompressionContext_t dctx;
    if (LZ4F_isError(LZ4F_createDecompressionContext(&dctx, LZ4F_VERSION))) {
        fprintf(stderr, "cant create dctx\n"); return 1;
    }

    size_t inPos = 0, inSize = 0;
    for (;;) {
        if (inPos == inSize) {                       /* need more input */
            inSize = fread(in, 1, CHUNK, stdin);
            inPos  = 0;
            if (inSize == 0) break;                  /* EOF */
        }

        size_t srcSize = inSize - inPos;
        size_t dstSize = CHUNK;
        const size_t ret = LZ4F_decompress(dctx, out, &dstSize,
                                     in + inPos, &srcSize, nullptr);
        if (LZ4F_isError(ret)) { fprintf(stderr, "decode\n"); return 2; }

        fwrite(out, 1, dstSize, stdout);
        inPos += srcSize;                            /* consume input */

        if (ret == 0 && inPos == inSize) break;      /* frame ended */
    }

    LZ4F_freeDecompressionContext(dctx);
    return 0;
}
