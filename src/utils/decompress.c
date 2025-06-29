#include <stdint.h>
#include <stdio.h>
#include "helper/lz4.h"

#define BLOCK 4096

int main(void)
{
    static char   src[LZ4_COMPRESSBOUND(BLOCK)];
    static char   dst[BLOCK];

    uint32_t n,c;
    while (fread(&n,1,sizeof n,stdin) == sizeof n &&
           fread(&c,1,sizeof c,stdin) == sizeof c)
    {
        if (c>sizeof src) { fputs("size field bogus\n",stderr); return 2; }
        if (fread(src,1,c,stdin)!=c) { fputs("trunc block\n",stderr);return 3;}

        int got=LZ4_decompress_safe(src,dst,c,n);
        if (got<0||got!=(int)n){fprintf(stderr,"decode %d\n",got);return 4;}

        fwrite(dst,1,n,stdout);
    }
}
