#ifndef SEARCHDATA_H
#define SEARCHDATA_H

typedef struct {
    long offset;
    unsigned int size;
} DataChunk;

DataChunk findDataChunk(unsigned char *data, long size);

#endif