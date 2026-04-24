#ifndef BUFFER_H
#define BUFFER_H

typedef struct {
    unsigned char *data;   // raw bytes
    long           size;   // how many bytes
} FileBuffer;

FileBuffer readFile       (const char *filename);
void       freeFileBuffer (FileBuffer *fb);

#endif
