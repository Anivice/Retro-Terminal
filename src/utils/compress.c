#include <stdint.h>
#include <stdio.h>
#include "core/lz4.h"

#define BLOCK 4096

int main(void)
{
    static char   src[BLOCK];
    static char   dst[LZ4_COMPRESSBOUND(BLOCK)];
    for (;;)
    {
        uint32_t n = fread(src,1,BLOCK,stdin);
        if (!n) break;

        uint32_t c = LZ4_compress_default(src,dst,n,sizeof dst);
        if (!c) { fputs("compress fail\n",stderr); return 1; }

        fwrite(&n,1,sizeof n,stdout);
        fwrite(&c,1,sizeof c,stdout);
        fwrite(dst,1,c,stdout);
    }
}
