#include <stdio.h>
#include <stdlib.h>
#include "helper/lz4frame.h"

#define CHUNK 65536                        /* 64 KiB */

int main(void)
{
    unsigned char  in [CHUNK];
    size_t         max = LZ4F_compressBound(CHUNK, NULL);   /* worst-case */
    unsigned char *out = malloc(max);

    LZ4F_compressionContext_t cctx;
    if (LZ4F_isError(LZ4F_createCompressionContext(&cctx, LZ4F_VERSION))) {
        fprintf(stderr, "cant create ctx\n"); return 1;
    }

    size_t n = LZ4F_compressBegin(cctx, out, max, NULL);
    fwrite(out, 1, n, stdout);

    while ((n = fread(in, 1, CHUNK, stdin))) {
        size_t outSize = LZ4F_compressUpdate(cctx, out, max, in, n, NULL);
        if (LZ4F_isError(outSize)) { fprintf(stderr, "compress\n"); return 2; }
        fwrite(out, 1, outSize, stdout);
    }

    n = LZ4F_compressEnd(cctx, out, max, NULL);             /* adds footer */
    fwrite(out, 1, n, stdout);

    LZ4F_freeCompressionContext(cctx);
    free(out);
    return 0;
}
