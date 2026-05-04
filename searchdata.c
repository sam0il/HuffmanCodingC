#include <stdio.h>
#include "searchdata.h"

DataChunk findDataChunk(unsigned char *data, long size) {
    DataChunk result;
    result.offset = -1;
    result.size   = 0;

    for (long i = 0; i < size - 8; i++) {
        if (data[i]     == 'd' &&
            data[i + 1] == 'a' &&
            data[i + 2] == 't' &&
            data[i + 3] == 'a') {

            unsigned int chunkSize =
                data[i + 4] |
                (data[i + 5] << 8) |
                (data[i + 6] << 16) |
                (data[i + 7] << 24);

            printf("Found 'data' chunk at byte %ld (size = %u bytes)\n",
                   i, chunkSize);

            result.offset = i + 8;
            result.size   = chunkSize;

            return result;
        }
    }

    return result;
}